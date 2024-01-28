#include "hzpch.h"

#include "Calibur/Renderer/SceneRenderer.h"

namespace Calibur
{
	namespace Utils
	{
		std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projView)
		{
			const auto inv = glm::inverse(projView);
			std::vector<glm::vec4> frustumCorners;

			for (uint32_t x = 0; x < 2; x++)
			{
				for (uint32_t y = 0; y < 2; y++)
				{
					for (uint32_t z = 0; z < 2; z++)
					{
						const glm::vec4 pt =
							inv * glm::vec4(
								2.0f * x - 1.0f, 
								2.0f * y - 1.0f,
								2.0f * z - 1.0f,
								1.0f);
						frustumCorners.push_back(pt / pt.w);
					}
				}
			}
			return frustumCorners;
		}

		std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
		{
			return getFrustumCornersWorldSpace(proj * view);
		}
		
		// Get a view projection matrix according near and far plane
		glm::mat4 GetLightSpaceMatrix(const float nearPlane, const float farPlane, const glm::vec3& lightDir, const SceneRenderCamera& camera)
		{
			glm::mat4 proj = glm::perspective(
				glm::radians(camera.Fov), camera.AspectRatio, nearPlane, farPlane
			);
			// Get the frustum corners in world space
			std::vector<glm::vec4> corners = Utils::getFrustumCornersWorldSpace(
			proj,
			camera.ViewMatrix);

			// Calculate light view and proj matrix
			glm::vec3 center{ 0.0, 0.0, 0.0 };
			for (auto& corner : corners)
			{
				center += glm::vec3(corner);
			}
			center /= corners.size();

			const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0, 1.0, 0.0));

			// Transform corner world position to light space
			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::lowest();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::lowest();
			float minZ = std::numeric_limits<float>::max();
			float maxZ = std::numeric_limits<float>::lowest();
			for (const auto& v : corners)
			{
				const auto trf = lightView * v;
				minX = std::min(minX, trf.x);
				maxX = std::max(maxX, trf.x);
				minY = std::min(minY, trf.y);
				maxY = std::max(maxY, trf.y);
				minZ = std::min(minZ, trf.z);
				maxZ = std::max(maxZ, trf.z);
			}

			const float zMult = 10.0f;
			if (minZ < 0.0f) minZ *= zMult;
			else minZ /= zMult;
			if (maxZ < 0.0f) maxZ /= zMult;
			else maxZ *= zMult;

			const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
			return lightProjection * lightView;
		}

		// Get all light space matrxi accroding to near and far plane
		std::vector<glm::mat4> GetLightSpaceMatrices(std::vector<float> csmLevels, const glm::vec3& lightDir, const SceneRenderCamera& camera)
		{
			std::vector<glm::mat4> ret;
			for (size_t i = 0; i < csmLevels.size() - 1; i++)
			{
				ret.push_back(GetLightSpaceMatrix(csmLevels[i], csmLevels[i + 1], lightDir, camera));
			}
			return ret;
		}
	}

	static Ref<UniformBuffer> s_DirectionalLightUniformBuffer;
	static Ref<UniformBuffer> s_PointLightUniformBuffer;

	SceneRenderer::SceneRenderer(Ref<Scene> scene)
		: m_Scene(scene) 
	{
		m_CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraUBData), 0);
		m_TransformBuffer = UniformBuffer::Create(sizeof(glm::mat4), 1);

		s_DirectionalLightUniformBuffer = UniformBuffer::Create(sizeof(DirectionalLightUBData), 3);
		s_PointLightUniformBuffer = UniformBuffer::Create(sizeof(PointLightUBData), 4);

		// Init Csm data
		TextureSpecification specTex;
		specTex.Format = ImageFormat::DEPTH32F;
		specTex.Filter = TextureFilter::Nearest;
		specTex.Height = 2048;
		specTex.Width = 2048;
		specTex.ArraySize = 5;
		specTex.Wrap = TextureWrap::ClampToBorder;
		m_CSMTextureArray = Texture2DArray::Create(specTex);

		m_DirCSMShader = Renderer::GetShaderLibrary()->Get("DirLightCSM");
		FramebufferSpecification specFbo;
		specFbo.Height = 2048;
		specFbo.Width = 2048;
		m_CSMFramebuffer = Framebuffer::Create(specFbo);
		m_CSMFramebuffer->SetDepthAttachment(m_CSMTextureArray->GetRendererID());

		m_LightMatricesBuffer = UniformBuffer::Create(sizeof(glm::mat4) * 16, 5);
	}

	void SceneRenderer::BeginScene(const SceneRenderCamera& camera)
	{
		CameraUB.CameraPosition = glm::vec4(camera.position, 1.0);
		CameraUB.ViewProjectionMatrix = camera.camera.GetProjection() * camera.ViewMatrix;

		m_CameraUniformBuffer->SetData(&CameraUB, sizeof(CameraUBData));
		// Save current scene data
		m_SceneRenderCamera = camera;
	}

	void SceneRenderer::SubmitLight(SceneLightData& lightData)
	{
		for (uint32_t id = 0; id < lightData.DirectionalLightCount; id++)
		{
			DirectionalLightUB.lights[id] = lightData.DirectionalLights[id];
			DirectionalLightUB.Count++;
		}
		lightData.DirectionalLightCount = 0;

		for (uint32_t id = 0; id < lightData.PointLights.size(); id++)
		{
			PointLightUB.lights[id] = lightData.PointLights[id];
			PointLightUB.Count++;
		}

		s_DirectionalLightUniformBuffer->SetData(&DirectionalLightUB, sizeof(DirectionalLightUBData));
		s_PointLightUniformBuffer->SetData(&PointLightUB, sizeof(PointLightUBData));
		
		// Generate Csm for directional map
		GenerateShadowMap(lightData);
	}
	
	// Generate CSM for Directional light
	// Now only support one light
	void SceneRenderer::GenerateShadowMap(const SceneLightData& lightData)
	{
		const DirectionalLight& light = lightData.DirectionalLights[0];

		// Set CSM level, have shadowCascadeLevels.size() - 1 levels.
		float cameraFarPlane = m_SceneRenderCamera.Far;
		float cameraNearPlane = m_SceneRenderCamera.Near;
		static std::vector<float> shadowCascadeLevels{cameraNearPlane, cameraFarPlane / 50.0f, cameraFarPlane / 25.0f, cameraFarPlane / 10.0f, cameraFarPlane / 2.0f, cameraFarPlane };

		// Set light matrices uniform buffer
		std::vector<glm::mat4>& lightMatrices = Utils::GetLightSpaceMatrices(shadowCascadeLevels, light.Direction, m_SceneRenderCamera);
		for (size_t i = 0; i < lightMatrices.size(); i++)
		{
			m_LightMatricesBuffer->SetData(&lightMatrices[i], sizeof(glm::mat4), i * sizeof(glm::mat4));
		}
		
		m_ActiveFramebuffer->Unbind();
		m_CSMFramebuffer->Bind();
		RenderCommand::SetViewport(0, 0, m_CSMTextureArray->GetWidth(), m_CSMTextureArray->GetHeight());
		RenderCommand::Clear();

		m_Scene->RenderScene2D();
		m_Scene->RenderScene3D(m_DirCSMShader);

		m_CSMFramebuffer->Unbind();
		m_ActiveFramebuffer->Bind();
		RenderCommand::SetViewport(0, 0, m_ActiveFramebuffer->GetWidth(), m_ActiveFramebuffer->GetHeight());
	}

	void SceneRenderer::EndScene()
	{
	}
}
