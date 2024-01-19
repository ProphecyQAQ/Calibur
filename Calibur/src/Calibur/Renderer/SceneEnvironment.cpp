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

	/*
	* @ param path: the path of the equirectangular map
	* @ param isVerticalFlip: whether the image is vertically flipped
	*/
	SceneEnvironment::SceneEnvironment(const std::string& path, bool isVerticalFlip)
	{
		HZ_PROFILE_FUNCTION();

		//Init Cube 
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

		// Init textures
		m_EquirectangularMap = Texture2D::Create(path, isVerticalFlip);
		m_Skybox = TextureCube::Create(512, 512);
		m_IrradianceMap = TextureCube::Create(32, 32);

		m_SkyboxShader = Renderer::GetShaderLibrary()->Get("Skybox");
		m_EquirectangularToCubemapShader = Renderer::GetShaderLibrary()->Get("EquirectangularToCube");
		m_IrradianceShader = Renderer::GetShaderLibrary()->Get("irradianceConvolution");

		// Set up the framebuffer
		FramebufferSpecification fbSpec;
		fbSpec.Width = 512;
		fbSpec.Height = 512;
		fbSpec.Samples = 1;
		fbSpec.Attachments = {FramebufferTextureFormat::DEPTH24STENCIL8};

		m_CaptureFramebuffer = Framebuffer::Create(fbSpec);

		DrawEquirectangularToCubemap(m_EquirectangularMap);
		DrawIrradianceMap();
	}

	void SceneEnvironment::DrawEquirectangularToCubemap(Ref<Texture2D> equirectangularMap)
	{
		m_CaptureFramebuffer->Bind();

		RenderCommand::SetViewport(0, 0, 512, 512);
		s_Cube->vao->Bind();

		uint32_t skyboxId = m_Skybox->GetRendererID();
		for (size_t i = 0; i < 6; i++)
		{
			m_EquirectangularToCubemapShader->Bind();
			equirectangularMap->Bind(0);

			glm::mat4 viewProjecion = captureProjection * captureViews[i];
			s_Cube->camera->SetData(&viewProjecion, sizeof(glm::mat4));
			m_CaptureFramebuffer->SetRenderTargetToCubeMap(skyboxId, i);

			RenderCommand::Clear();

			Renderer::Submit(m_EquirectangularToCubemapShader, s_Cube->vao);
		}

		s_Cube->vao->Unbind();
		m_CaptureFramebuffer->Unbind();
	}

	void SceneEnvironment::DrawIrradianceMap()
	{
		m_CaptureFramebuffer->Bind();

		RenderCommand::SetViewport(0, 0, 32, 32);
		s_Cube->vao->Bind();

		uint32_t IrradianceId = m_IrradianceMap->GetRendererID();
		for (size_t i = 0; i < 6; i++)
		{
			m_IrradianceShader->Bind();
			m_Skybox->Bind(0);

			glm::mat4 viewProjecion = captureProjection * captureViews[i];
			s_Cube->camera->SetData(&viewProjecion, sizeof(glm::mat4));
			m_CaptureFramebuffer->SetRenderTargetToCubeMap(IrradianceId, i);

			RenderCommand::Clear();

			Renderer::Submit(m_IrradianceShader, s_Cube->vao);
		}

		s_Cube->vao->Unbind();
		m_CaptureFramebuffer->Unbind();
		
	}

	
	

}
