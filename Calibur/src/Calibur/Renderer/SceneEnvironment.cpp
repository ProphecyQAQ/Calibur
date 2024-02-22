#include "hzpch.h"
#include "Calibur/Renderer/SceneEnvironment.h"

#include <glm/glm.hpp>

namespace Calibur
{
	static float boxVertices[24] =
	{
		+1, +1, +1,
		-1, +1, +1,
		-1, -1, +1,
		+1, -1, +1,
		+1, +1, -1,
		-1, +1, -1,
		-1, -1, -1,
		+1, -1, -1,
	};

	static uint32_t boxIndices[6 * 6] =  // in fact, uint8_t is enough
	{
		1,5,2, 2,5,6,  // left
		3,7,0, 0,7,4,  // right
		2,6,3, 3,6,7,  // back
		0,4,1, 1,4,5,  // front
		6,5,7, 7,5,4,  // bottom
		1,2,0, 0,2,3,  // top
	};
	
	static glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	static glm::mat4 captureViews[] = 
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		 glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		 glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		 glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		 glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	static struct CubeData
	{
		Ref<VertexArray> vao;
		Ref<VertexBuffer> vbo;
		Ref<IndexBuffer> ibo;
		Ref<UniformBuffer> camera;
	}* s_Cube;
	
	Ref<Texture2D> SceneEnvironment::s_BrdfLut = nullptr;

	/*
	* @ param path: the path of the equirectangular map
	* @ param isVerticalFlip: whether the image is vertically flipped
	*/
	SceneEnvironment::SceneEnvironment(const std::string& path, bool isVerticalFlip)
	{
		HZ_PROFILE_FUNCTION();

		//Init Cube 
		if (s_Cube == nullptr) 
		{
			s_Cube = new CubeData();
			// Set up the box
			s_Cube->vbo = VertexBuffer::Create(boxVertices, sizeof(boxVertices));
			s_Cube->ibo = IndexBuffer::Create(boxIndices, sizeof(boxIndices));
			BufferLayout layout = {{ ShaderDataType::Float3, "a_Position" }};
			s_Cube->vbo->SetLayout(layout);

			s_Cube->vao = VertexArray::Create();
			s_Cube->vao->SetIndexBuffer(s_Cube->ibo);
			s_Cube->vao->AddVertexBuffer(s_Cube->vbo);

			s_Cube->camera = UniformBuffer::Create(sizeof(glm::mat4), 30);
		}

		// Init textures
		TextureSpecification spec;
		spec.isVerticalFlip = isVerticalFlip;
		m_EquirectangularMap = Texture2D::Create(spec, path);

		spec.Width = 32;
		spec.Height = 32;
		spec.Format = ImageFormat::RGB16F;
		spec.Filter = TextureFilter::Linear;
		spec.isGenerateMipMap = false;
		spec.isVerticalFlip = false;
		m_IrradianceMap = TextureCube::Create(spec);
		
		spec.Width = 128;
		spec.Height = 128;
		spec.isGenerateMipMap = true;
		spec.Format = ImageFormat::RGB16F;
		m_PrefilterMap = TextureCube::Create(spec);

		spec.Width = 512;
		spec.Height = 512;
		spec.Format = ImageFormat::RGB16F;
		spec.Wrap = TextureWrap::ClampToEdge;
		spec.isGenerateMipMap = true;
		m_Skybox = TextureCube::Create(spec);

		// Init shader
		m_SkyboxShader = Renderer::GetShaderLibrary()->Get("Skybox");
		m_EquirectangularToCubemapShader = Renderer::GetShaderLibrary()->Get("EquirectangularToCube");
		m_IrradianceShader = Renderer::GetShaderLibrary()->Get("irradianceConvolution");
		m_PrefilterShader = Renderer::GetShaderLibrary()->Get("prefilteredConvolution");

		DrawEquirectangularToCubemap(m_EquirectangularMap);
		DrawIrradianceMap();
		DrawPrefilterMap();

		// Init brdf Lut
		if (!s_BrdfLut)
		{
			DrawBrdfLut();
		}

		// Set Environment map
		GetPreFilterMap()->Bind(5);
		GetIrradianceMap()->Bind(6);
	}

	void SceneEnvironment::DrawEquirectangularToCubemap(Ref<Texture2D> equirectangularMap)
	{
		m_FboSpec.Width = 512;
		m_FboSpec.Height = 512;
		m_FboSpec.Samples = 1;
		m_FboSpec.Attachments = {FramebufferTextureFormat::DEPTH24STENCIL8};
		Ref<Framebuffer> fbo = Framebuffer::Create(m_FboSpec);

		// Set framebuffer
		fbo->Bind();

		RenderCommand::SetViewport(0, 0, 512, 512);
		s_Cube->vao->Bind();

		uint32_t skyboxId = m_Skybox->GetRendererID();
		for (size_t i = 0; i < 6; i++)
		{
			m_EquirectangularToCubemapShader->Bind();
			equirectangularMap->Bind(0);

			glm::mat4 viewProjecion = captureProjection * captureViews[i];
			s_Cube->camera->SetData(&viewProjecion, sizeof(glm::mat4));
			fbo->SetRenderTargetToTextureCube(skyboxId, i);

			RenderCommand::Clear();

			Renderer::Submit(m_EquirectangularToCubemapShader, s_Cube->vao);
		}

		m_Skybox->GenerateMipmap();

		s_Cube->vao->Unbind();
		fbo->Unbind();
	}

	void SceneEnvironment::DrawIrradianceMap()
	{
		// Set framebuffer
		m_FboSpec.Width = 32;
		m_FboSpec.Height = 32;
		m_FboSpec.Samples = 1;
		m_FboSpec.Attachments = {FramebufferTextureFormat::DEPTH24STENCIL8};
		Ref<Framebuffer> fbo = Framebuffer::Create(m_FboSpec);

		fbo->Bind();

		RenderCommand::SetViewport(0, 0, 32, 32);
		s_Cube->vao->Bind();

		uint32_t IrradianceId = m_IrradianceMap->GetRendererID();
		for (size_t i = 0; i < 6; i++)
		{
			m_IrradianceShader->Bind();
			m_Skybox->Bind(0);

			glm::mat4 viewProjecion = captureProjection * captureViews[i];
			s_Cube->camera->SetData(&viewProjecion, sizeof(glm::mat4));
			fbo->SetRenderTargetToTextureCube(IrradianceId, i);

			RenderCommand::Clear();

			Renderer::Submit(m_IrradianceShader, s_Cube->vao);
		}

		s_Cube->vao->Unbind();
		fbo->Unbind();	
	}

	void SceneEnvironment::DrawPrefilterMap()
	{
		//Set framebuffer 

		uint32_t maxMipLevels = 5;
		Ref<UniformBuffer> roughnessBuffer = UniformBuffer::Create(sizeof(float), 31);
		uint32_t prefilter = m_PrefilterMap->GetRendererID();
		for (uint32_t mip = 0; mip < maxMipLevels; mip++)
		{
			m_FboSpec.Width = static_cast<uint32_t>(128 * std::pow(0.5, mip));
			m_FboSpec.Height = static_cast<uint32_t>(128 * std::pow(0.5, mip));
			m_FboSpec.Samples = 1;
			m_FboSpec.Attachments = {FramebufferTextureFormat::DEPTH24STENCIL8};
			Ref<Framebuffer> fbo = Framebuffer::Create(m_FboSpec);
			RenderCommand::SetViewport(0, 0, m_FboSpec.Width, m_FboSpec.Width);

			fbo->Bind();

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			roughnessBuffer->SetData(&roughness, sizeof(float));
			
			for (uint32_t i = 0; i < 6; i++)
			{
				m_PrefilterShader->Bind();
				m_Skybox->Bind(0);

				glm::mat4 viewProjecion = captureViews[i];
				s_Cube->camera->SetData(&viewProjecion, sizeof(glm::mat4));
				fbo->SetRenderTargetToTextureCube(prefilter, i, mip);
				
				RenderCommand::Clear();
				Renderer::Submit(m_PrefilterShader, s_Cube->vao);
			}
			fbo->Unbind();
		}
	}

	// Static method
	void SceneEnvironment::DrawBrdfLut()
	{
		TextureSpecification lutSpec;
		lutSpec.Height = 512;
		lutSpec.Width = 512;
		lutSpec.Format = ImageFormat::RG16F;
		lutSpec.Wrap = TextureWrap::ClampToEdge;
		lutSpec.isGenerateMipMap = false;
		s_BrdfLut = Texture2D::Create(lutSpec);
		
		// Rectangle data
		float vertices[] = {
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f  
		};
		uint32_t indices[] = {
			0, 1, 3,
			1, 2, 3
		};
		Ref<VertexArray> vao = VertexArray::Create();
		Ref<VertexBuffer> vbo = VertexBuffer::Create(vertices, sizeof(vertices));
		Ref<IndexBuffer> ibo = IndexBuffer::Create(indices, sizeof(indices));
		vbo->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
			});
		vao->AddVertexBuffer(vbo);
		vao->SetIndexBuffer(ibo);

		// Shader
		Ref<Shader> shader = Renderer::GetShaderLibrary()->Get("BrdfLut");
		
		FramebufferSpecification FboSpec;
		FboSpec.Height = 512;
		FboSpec.Width = 512;
		FboSpec.Attachments = {FramebufferTextureFormat::DEPTH24STENCIL8};
		Ref<Framebuffer> fbo = Framebuffer::Create(FboSpec);
		RenderCommand::SetViewport(0, 0, 512, 512);
		
		fbo->Bind();

		fbo->SetRenderTargetToTexture2D(s_BrdfLut->GetRendererID());
		RenderCommand::Clear();
		Renderer::Submit(shader, vao);

		vao->Unbind();
		fbo->Unbind();

		s_BrdfLut->Bind(7);
	}
}
