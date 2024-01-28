#pragma once

#include "entt.hpp"
#include "Calibur/Core/TimeStep.h"
#include "Calibur/Renderer/EditorCamera.h"
#include "Calibur/Renderer/Shader.h"
#include "Calibur/Renderer/UniformBuffer.h"
#include "Calibur/Renderer/SceneEnvironment.h"

#include <glm/glm.hpp>

namespace Calibur
{
	class Entity;
	class SceneRenderer;

	// Using glm::vec4 to Padding for radiance and position

	struct DirectionalLight
	{
		glm::vec3 Radiance{ 0.0f };
		float Intensity = 0.0f;
		glm::vec3 Direction{ 0.0f };

		void clear() {
			Radiance = glm::vec4(0.0f);
			Direction = glm::vec4(0.0f);
			Intensity = 0.0f;
		}
	};
	
	struct PointLight
	{
		glm::vec3 Radiance{ 0.0f };
		float Intensity = 0.0f;
		glm::vec3 Position{ 0.0f};
		float Radius = 25.0f;
		float SourceSize = 0.1f;
		uint32_t CastShadow = 0;
	};

	struct SceneLightData
	{
		uint32_t DirectionalLightCount = 0;
		DirectionalLight DirectionalLights[4];
		std::vector<PointLight> PointLights;
	};

	class Scene : public std::enable_shared_from_this<Scene>
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

		void RenderScene2D();
		void RenderScene3D(Ref<Shader> shader = nullptr);
	
		Entity GetPrimaryCameraEntity();

		Ref<SceneEnvironment> m_SceneEnv;
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth, m_ViewportHeight;
		
		Ref<UniformBuffer> m_MaterialUniform;
		Ref<SceneRenderer> m_Renderer;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}
