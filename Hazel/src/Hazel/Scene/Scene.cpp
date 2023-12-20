#include "hzpch.h"
#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Components.h"
#include "Hazel/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

namespace Hazel
{
	Scene::Scene()
	{
		#if 0
		entt::entity entity =  m_Registry.create(); // return a uint32_t 

		m_Registry.emplace<TransformComponent>(entity);

		if (m_Registry.try_get<TransformComponent>(entity))
			TransformComponent& transform = m_Registry.get<TransformComponent>(entity);
		
		auto view = m_Registry.view<TransformComponent>();
		#endif
	}

	Scene::~Scene()
	{
	}

	entt::entity Scene::CreateEntity()
	{
		return m_Registry.create();
	}

	void Scene::OnUpdate(TimeStep ts)
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponenet>);
		for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponenet>(entity);
			Renderer2D::DrawQuad((glm::mat4)transform, (glm::vec4)sprite);
		}
	}
}