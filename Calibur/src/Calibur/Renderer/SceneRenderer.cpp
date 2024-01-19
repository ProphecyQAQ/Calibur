#include "hzpch.h"

#include "Calibur/Renderer/SceneRenderer.h"

namespace Calibur
{
	SceneRenderer::SceneRenderer(Ref<Scene> scene)
		: m_Scene(scene) 
	{
		m_CameraUB = UniformBuffer::Create(sizeof(CameraData), 0);
	}

	void SceneRenderer::BeginScene(const SceneRenderCamera& camera)
	{
		m_CameraUBData.CameraPosition = glm::vec4(camera.position, 1.0);
		m_CameraUBData.ViewProjectionMatrix = camera.camera.GetProjection() * camera.ViewMatrix;

		m_CameraUB->SetData(&m_CameraUBData, sizeof(CameraData));
	}

	void SceneRenderer::EndScene()
	{
	}

}
