#pragma once

#include "entt.hpp"
#include "Calibur/Core/TimeStep.h"
#include "Calibur/Renderer/EditorCamera.h"
#include "Calibur/Renderer/Shader.h"

namespace Calibur
{
	class Entity;

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

		// !TODO: Remove shader
		Ref<Shader> m_MeshShader;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}
