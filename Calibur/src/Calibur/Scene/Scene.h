#pragma once

#include "entt.hpp"
#include "Calibur/Core/TimeStep.h"
#include "Calibur/Renderer/EditorCamera.h"
#include "Calibur/Renderer/Shader.h"
#include "Calibur/Renderer/UniformBuffer.h"

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

		void OnUpdateEditor(TimeStep ts, EditorCamera& camera);
		void OnUpdateRuntime(TimeStep ts);
		void OnViewportResize(uint32_t width, uint32_t height);
		
		Entity GetPrimaryCameraEntity();
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
