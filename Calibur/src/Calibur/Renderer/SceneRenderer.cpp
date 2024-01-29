#include "hzpch.h"

#include "Calibur/Renderer/SceneRenderer.h"

namespace Calibur
{

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
		m_CascadeData.resize(m_DirCSMCount);

		m_LightMatricesBuffer = UniformBuffer::Create(sizeof(glm::mat4) * m_DirCSMCount + m_DirCSMCount * sizeof(glm::vec4), 5);
	}

	void SceneRenderer::BeginScene(const SceneRenderCamera& camera)
	{
		CameraUB.CameraPosition = glm::vec4(camera.position, 1.0);
		CameraUB.ViewMatrix = camera.ViewMatrix;
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
		if (lightData.DirectionalLightCount == 0) return;
		
		glm::vec3 lightDirection = lightData.DirectionalLights[0].Direction;

		CalculateCascades(lightDirection);
		
		for (size_t i = 0; i < m_CascadeData.size(); i++)
		{
			m_LightMatricesBuffer->SetData(&m_CascadeData[i], sizeof(glm::mat4), i * sizeof(glm::mat4));
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
		const glm::vec4 origin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		glm::mat4 viewProj = m_SceneRenderCamera.camera.GetProjection() * viewMatrix;

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
			float d = 0.92 * (log - uniform) + uniform;
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
			glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, maxExtents.z - minExtents.z + 0.0f);

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

	void SceneRenderer::EndScene()
	{
		// Clear Light ub data
		DirectionalLightUB.Count = 0;
		PointLightUB.Count = 0;
	}
}
