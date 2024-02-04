#pragma once

#include "Calibur/Core/Base.h"
#include "Calibur/Scene/Scene.h"
#include "Calibur/Scene/Entity.h"

namespace Calibur
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& context);
		Entity GetSelectedEntity() { return m_SelectionContext; }	
		void SetSelectedEntity(Entity entity);

		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}
