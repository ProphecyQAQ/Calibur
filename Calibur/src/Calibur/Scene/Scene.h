#pragma once

#include "entt.hpp"
#include "Calibur/Core/TimeStep.h"
#include "Calibur/Renderer/EditorCamera.h"
#include "Calibur/Renderer/Shader.h"
#include "Calibur/Renderer/UniformBuffer.h"
#include "Calibur/Renderer/SceneEnvironment.h"
#include "Calibur/Renderer/SceneRenderer.h"

#include <glm/glm.hpp>

namespace Calibur
{
	class Entity;

	struct DirectionalLight
	{
		glm::vec3 Radiance{ 0.0f, 0.0f, 0.0f };
		glm::vec3 Direction{ 0.0f, 0.0f, 0.0f };
		float Intensity = 0.0f;
	};

	struct SceneLightData
	{
		DirectionalLight DirectionalLights;
		//int DirectionalLightIndex = 0;
	};

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		entt::registry& Reg() { return m_Registry; }

		void OnUpdateEditor(Ref<SceneRenderer> renderer, TimeStep ts, EditorCamera& camera);
		void OnUpdateRuntime(Ref<SceneRenderer> renderer, TimeStep ts);
		void OnViewportResize(uint32_t width, uint32_t height);
		
		Entity GetPrimaryCameraEntity();

		Ref<SceneEnvironment> m_SceneEnv;
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth, m_ViewportHeight;
		
		Ref<UniformBuffer> m_TransformBuffer;
		Ref<UniformBuffer> m_MaterialUniform;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}
