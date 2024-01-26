#pragma once

#include "Calibur/Scene/Scene.h"
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

	class SceneRenderer
	{
	public:
		SceneRenderer(Ref<Scene> scene);
		~SceneRenderer() = default;

		void BeginScene(const SceneRenderCamera &camera);
		void SubmitLight(SceneLightData& lightData);
		void EndScene();

		void SetScene(Ref<Scene> scene) { m_Scene = scene; } // not work for raw pointer

		Ref<UniformBuffer>& GetTransformUB() { return m_TransformBuffer; }
	private:

		Ref<Scene> m_Scene;

		//Unfirom buffer
		Ref<UniformBuffer> m_CameraUniformBuffer;
		Ref<UniformBuffer> m_TransformBuffer;

	private:

		struct CameraUBData
		{
			glm::mat4 ViewProjectionMatrix;
			glm::vec4 CameraPosition;
		} CameraUB;

		struct DirectionalLightUBData
		{
			uint32_t Count = 0;
			glm::vec3 Padding{ 0.0 };
			DirectionalLight lights[4];
		} DirectionalLightUB;

		struct PointLightUBData
		{
			uint32_t Count = 0;
			glm::vec3 Padding{ 0.0 };
			PointLight lights[16];
		} PointLightUB;

	};
}
