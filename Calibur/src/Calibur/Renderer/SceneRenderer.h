#pragma once

#include "Calibur/Renderer/Camera.h"
#include "Calibur/Renderer/UniformBuffer.h"

#include <glm/glm.hpp>

namespace Calibur
{
	struct SceneRenderCamera
	{
		Camera camera;
		glm::mat4 ViewMatrix;
		glm::vec3 position;
	};

	class Scene;

	class SceneRenderer
	{
	public:
		SceneRenderer(Ref<Scene> scene);
		~SceneRenderer() = default;

		void BeginScene(const SceneRenderCamera &camera);
		void EndScene();

		void SetScene(Ref<Scene> scene) { m_Scene = scene; } // not work for raw pointer
	private:

		struct CameraData
		{
			glm::mat4 ViewProjectionMatrix;
			glm::vec4 CameraPosition;
		} m_CameraUBData;

		Ref<Scene> m_Scene;

		//Unfirom buffer
		Ref<UniformBuffer> m_CameraUB;
	};
}
