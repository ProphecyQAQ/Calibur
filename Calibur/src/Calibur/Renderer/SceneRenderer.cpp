#include "hzpch.h"
#include "Calibur/Renderer/SceneRenderer.h"

namespace Calibur
{
	static Ref<UniformBuffer> s_DirectionalLightUniformBuffer;
	static Ref<UniformBuffer> s_PointLightUniformBuffer;

	static struct RecData
	{	
		Ref<VertexArray> vao;
		Ref<VertexBuffer> vbo;
		Ref<IndexBuffer> ibo;
	}*s_RecData;

	static float vertices[] = {
		-1.0, -1.0, 0.0, 0.0, 0.0,
		1.0, -1.0, 0.0, 1.0, 0.0,
		1.0, 1.0, 0.0, 1.0, 1.0,
		-1.0, 1.0, 0.0, 0.0, 1.0
	};

	static uint32_t indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	SceneRenderer::SceneRenderer(Ref<Scene> scene)
		: m_Scene(scene) 
	{
		if (!s_RecData)
		{
			s_RecData = new RecData();
			s_RecData->vao = VertexArray::Create();
			s_RecData->vbo = VertexBuffer::Create(vertices, sizeof(vertices));	
			s_RecData->vbo->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" }
			});
			s_RecData->vao->AddVertexBuffer(s_RecData->vbo);
			s_RecData->ibo = IndexBuffer::Create(indices, sizeof(indices));
			s_RecData->vao->SetIndexBuffer(s_RecData->ibo);
		}

		m_CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraUBData), 0);
		m_TransformBuffer = UniformBuffer::Create(2 * sizeof(glm::mat4), 1);
		m_TaaParamBuffer = UniformBuffer::Create(sizeof(TaaParamUBData), 6);
		m_FinalBoneMatricesBuffer = UniformBuffer::Create(sizeof(glm::mat4) * 100, 7);

		s_DirectionalLightUniformBuffer = UniformBuffer::Create(sizeof(DirectionalLightUBData), 3);
		s_PointLightUniformBuffer = UniformBuffer::Create(sizeof(PointLightUBData), 4);

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = {FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::RG16F,FramebufferTextureFormat::DEPTH32FSTENCIL8};
		fbSpec.Width = 1920;
		fbSpec.Height = 1080;
		fbSpec.Samples = 1;
		m_MainFramebuffer = Framebuffer::Create(fbSpec);

		// Init Csm data
		TextureSpecification specTex;
		specTex.Format = ImageFormat::DEPTH32F;
		specTex.Filter = TextureFilter::Nearest;
		specTex.Height = 2048;
		specTex.Width = 2048;
		specTex.ArraySize = 4;
		specTex.Wrap = TextureWrap::ClampToBorder;
		m_CSMTextureArray = Texture2DArray::Create(specTex);

		m_DirCSMShader = Renderer::GetShaderLibrary()->Get("DirLightCSM");
		FramebufferSpecification specFbo;
		specFbo.Height = 2048;
		specFbo.Width = 2048;
		m_CSMFramebuffer = Framebuffer::Create(specFbo);
		m_CSMFramebuffer->SetDepthAttachment(m_CSMTextureArray->GetRendererID());
		m_CascadeData.resize(m_DirCSMCount);
		
		m_LightMatricesBuffer = UniformBuffer::Create(sizeof(glm::mat4) * m_DirCSMCount + m_DirCSMCount * sizeof(glm::vec4), 5);

		// Taa 
		TextureSpecification specTaaTex;
		specTaaTex.Format = ImageFormat::RGBA;
		specTaaTex.Width = 1920;
		specTaaTex.Height = 1080;
		specTaaTex.isGenerateMipMap = false;
		m_PreviousFrame = Texture2D::Create(specTaaTex);
		m_CurrentFrame = Texture2D::Create(specTaaTex);
		specTaaTex.Format = ImageFormat::RG16F;
		m_MotionVertor = Texture2D::Create(specTaaTex);
		specTaaTex.Format = ImageFormat::DEPTH32FSTENCIL8;
		m_CurrentDepth = Texture2D::Create(specTaaTex);
	}

	void SceneRenderer::BeginScene(const SceneRenderCamera& camera)
	{
		// Set Main framebuffer
		m_MainFramebuffer->Bind();
		RenderCommand::SetViewport(0, 0, m_MainFramebuffer->GetWidth(), m_MainFramebuffer->GetHeight());
		RenderCommand::Clear();
		m_MainFramebuffer->ClearAttachment(2, glm::vec2(0.0f));
		m_MainFramebuffer->ClearAttachment(1, -1);
		
		// Save current scene data
		m_SceneRenderCamera = camera;
		
		// Set Camera uniform data
		CameraUB.CameraPosition = glm::vec4(camera.position, 1.0);
		CameraUB.ViewMatrix = camera.ViewMatrix;
		CameraUB.ProjectionMatrix = camera.camera.GetProjection();
		CameraUB.ViewProjectionMatrix = camera.camera.GetProjection() * camera.ViewMatrix;
		
		m_CameraUniformBuffer->SetData(&CameraUB, sizeof(CameraUBData));

		// Set Taa date
		TaaParamUB.ScreenWidth = 1920.f;
		TaaParamUB.ScreenHeight = 1080.f;
		static uint32_t frameIndex = 0;
		frameIndex = (frameIndex + 1) % 8;
		TaaParamUB.FrameIndex = frameIndex;

		m_TaaParamBuffer->SetData(&TaaParamUB, sizeof(TaaParamUBData));
	}

	void SceneRenderer::SubmitLight(SceneLightData& lightData)
	{
		for (uint32_t id = 0; id < lightData.DirectionalLightCount; id++)
		{
			DirectionalLightUB.lights[id] = lightData.DirectionalLights[id];
			DirectionalLightUB.Count++;
		}

		for (uint32_t id = 0; id < lightData.PointLights.size(); id++)
		{
			PointLightUB.lights[id] = lightData.PointLights[id];
			PointLightUB.Count++;
		}

		s_DirectionalLightUniformBuffer->SetData(&DirectionalLightUB, sizeof(DirectionalLightUBData));
		s_PointLightUniformBuffer->SetData(&PointLightUB, sizeof(PointLightUBData));
		
		// Generate Csm for directional map
		glm::vec3 lightDirection = lightData.DirectionalLights[0].Direction;
		CalculateCascades(lightDirection);
		GenerateShadowMap(lightData);
	}
	
	// Generate CSM for Directional light
	// Now only support one light
	void SceneRenderer::GenerateShadowMap(const SceneLightData& lightData)
	{
		if (lightData.DirectionalLightCount == 0) return;		

		// Upload light viewProj matrix
		for (size_t i = 0; i < m_CascadeData.size(); i++)
		{
			m_LightMatricesBuffer->SetData(&m_CascadeData[i].ViewProj, sizeof(glm::mat4), i * sizeof(glm::mat4));
		}
		
		m_CSMFramebuffer->Bind();
		RenderCommand::SetViewport(0, 0, m_CSMTextureArray->GetWidth(), m_CSMTextureArray->GetHeight());
		RenderCommand::Clear();
		RenderCommand::SetFaceCulling(false);

		m_Scene->RenderScene2D();
		m_Scene->RenderScene3D(m_DirCSMShader);

		m_CSMFramebuffer->Unbind();

		// Uplaod Shadow data
		for (size_t i = 0; i < m_CascadeData.size(); i++)
		{
			m_LightMatricesBuffer->SetData(&m_CascadeData[i].SplitDepth, sizeof(float), m_DirCSMCount * sizeof(glm::mat4) + i * sizeof(glm::vec4));
		}
		m_CSMTextureArray->Bind(8);
	}

	void SceneRenderer::CalculateCascades(const glm::vec3& lightDirection)
	{
		glm::mat4 viewMatrix = m_SceneRenderCamera.ViewMatrix;
		glm::mat4 viewProj = m_SceneRenderCamera.camera.GetProjection() * viewMatrix;

		const glm::vec4 origin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		float cascadeSplitLambda = 0.95f;

		std:: vector<float> cascadeSplits(m_DirCSMCount);

		float nearPlane = m_SceneRenderCamera.Near;
		float farPlane = m_SceneRenderCamera.Far;
		float clipRange = farPlane - nearPlane;

		float minZ = nearPlane;
		float maxZ = nearPlane + clipRange;

		float range = maxZ - minZ;
		float ratio = maxZ / minZ;

		for (size_t i = 0; i < m_DirCSMCount; i++)
		{
			float p = (i + 1) / static_cast<float>(m_DirCSMCount);
			float log = minZ * std::pow(ratio, p);
			float uniform = minZ + range * p;
			float d = cascadeSplitLambda * (log - uniform) + uniform;
			cascadeSplits[i] = (d - nearPlane) / clipRange;
		}

		// Calculate orthographic projection matrix for each cascade
		float lastSplitDist = 0.0;
		for (size_t i = 0; i < m_DirCSMCount; i++)
		{
			float splitDist = cascadeSplits[i];
			
			glm::vec3 frustumCorners[8] =
			{
				glm::vec3(-1.0f,  1.0f, -1.0f),
				glm::vec3(1.0f,  1.0f, -1.0f),
				glm::vec3(1.0f, -1.0f, -1.0f),
				glm::vec3(-1.0f, -1.0f, -1.0f),
				glm::vec3(-1.0f,  1.0f,  1.0f),
				glm::vec3(1.0f,  1.0f,  1.0f),
				glm::vec3(1.0f, -1.0f,  1.0f),
				glm::vec3(-1.0f, -1.0f,  1.0f),
			};

			// Convert frustum corners to world space
			glm::mat4 invCam = glm::inverse(viewProj);
			for (size_t j = 0; j < 8; j++)
			{
				glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[j], 1.0f);
				frustumCorners[j] = invCorner / invCorner.w;
			}

			for (size_t j = 0; j < 4; j++)
			{
				glm::vec3 dist = frustumCorners[j + 4] - frustumCorners[j];
				frustumCorners[j + 4] = frustumCorners[j] + (dist * splitDist);
				frustumCorners[j] = frustumCorners[j] + (dist * lastSplitDist);
			}
	
			// Get frustum center
			glm::vec3 frustumCenter = glm::vec3(0.0f);
			for (size_t j = 0; j < 8; j++)
			{
				frustumCenter += frustumCorners[j];
			}
			frustumCenter /= 8.0f;

			float radius = 0.0f;
			for (size_t j = 0; j < 8; j++)
			{
				float distance = glm::length(frustumCorners[j] - frustumCenter);
				radius = std::max(radius, distance);
			}
			radius = std::ceil(radius * 16.0f) / 16.0f;

			glm::vec3 maxExtents = glm::vec3(radius);
			glm::vec3 minExtents = -maxExtents;
			
			glm::vec3 lightDir = -lightDirection;	
			glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, -200.0f, maxExtents.z - minExtents.z + 200.f);
			
			glm::mat4 shadowMatrix = lightOrthoMatrix * lightViewMatrix;
			float ShadowMapWidth = (float)m_CSMTextureArray->GetWidth();

			glm::vec4 shadowOrigin = (shadowMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) * ShadowMapWidth / 2.0f;
			glm::vec4 roundedOrigin = glm::round(shadowOrigin);
			glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
			roundOffset = roundOffset * 2.0f / ShadowMapWidth;
			roundOffset.z = 0.0f;
			roundOffset.w = 0.0f;
			
			lightOrthoMatrix[3] += roundOffset;

			m_CascadeData[i].ViewProj = lightOrthoMatrix * lightViewMatrix;
			m_CascadeData[i].View = lightViewMatrix;
			m_CascadeData[i].SplitDepth = (nearPlane + splitDist * clipRange) * -1.0f;

			lastSplitDist = cascadeSplits[i];
		}
	}

	void SceneRenderer::TaaPass()
	{
		Ref<Shader> shader = Renderer::GetShaderLibrary()->Get("Taa");

		// Taa pass
		m_CurrentFrame->CopyDataFromAnotherTexture(m_MainFramebuffer->GetColorAttachmentRendererID(0));
 		m_MotionVertor->CopyDataFromAnotherTexture(m_MainFramebuffer->GetColorAttachmentRendererID(2));
		m_CurrentDepth->CopyDataFromAnotherTexture(m_MainFramebuffer->GetDepthAttachmentRendererID());

		m_PreviousFrame->Bind(9);
		m_CurrentFrame->Bind(10);
		m_MotionVertor->Bind(11);
		m_CurrentDepth->Bind(12);

		Renderer::Submit(shader, s_RecData->vao);
		
		// Render to screen
		m_ActiveFramebuffer->Bind();
		RenderCommand::Clear();
		RenderCommand::SetViewport(0, 0, m_ActiveFramebuffer->GetWidth(), m_ActiveFramebuffer->GetHeight());

		Renderer::Submit(shader, s_RecData->vao);
	}

	void SceneRenderer::EndScene()
	{
		// Clear Light ub data
		DirectionalLightUB.Count = 0;
		PointLightUB.Count = 0;

		// Recoder previous data
		TaaParamUB.PreView = CameraUB.ViewMatrix;
		TaaParamUB.PreProjection = CameraUB.ProjectionMatrix;

		m_PreviousFrame->CopyDataFromAnotherTexture(m_MainFramebuffer->GetColorAttachmentRendererID(0));

		// Record previous transform
		auto& view = m_Scene->Reg().view<TransformComponent>();
		for (auto& entity : view)
		{
			Entity e = { entity, m_Scene.get() };
			auto& tc = e.GetComponent<TransformComponent>();
			tc.PreTransform = tc.GetTransform();
		}
	}
}
