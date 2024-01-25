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
		s_DirectionalLightUniformBuffer = UniformBuffer::Create(sizeof(DirectionalLightUBData), 3);
		s_PointLightUniformBuffer = UniformBuffer::Create(sizeof(PointLightUBData), 4);
	}

	void SceneRenderer::BeginScene(const SceneRenderCamera& camera)
	{
		CameraUB.CameraPosition = glm::vec4(camera.position, 1.0);
		CameraUB.ViewProjectionMatrix = camera.camera.GetProjection() * camera.ViewMatrix;

		m_CameraUniformBuffer->SetData(&CameraUB, sizeof(CameraUBData));
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

		//Cleraing the light data
		DirectionalLightUB.Count = 0;
		PointLightUB.Count = 0;
	}

	void SceneRenderer::EndScene()
	{
	}

}
