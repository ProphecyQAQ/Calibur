#pragma once

#include "entt.hpp"
#include "Hazel/Core/TimeStep.h"

namespace Hazel
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());

		entt::registry& Reg() { return m_Registry; }

		void OnUpdate(TimeStep ts);
		void OnViewportResize(uint32_t width, uint32_t height);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth, m_ViewportHeight;

		friend class Entity;
	};
}