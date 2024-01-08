#pragma once

#include "Calibur.h"
#include "Calibur/Core/Base.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

#include "Calibur/Renderer/EditorCamera.h"

namespace Calibur
{
	extern const std::filesystem::path g_AssetPath;

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(TimeStep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& e) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		
		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();
	private:
		OrthographicCameraController m_CameraController;

		Ref<Shader> m_FlatColorShader;
		Ref<VertexArray> m_SquareVA;
		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Entity m_Square;
		Entity m_CameraEntity;
		Entity m_SecondCamera;

		bool m_PrimaryCamera = false;

		glm::vec2 m_ViewportSize = { 0.f, 0.f };
		glm::vec2 m_ViewportBounds[2]; //global viewport bounds
		EditorCamera m_EditorCamera;
		Entity m_HoveredEntity;

		Ref<Texture2D> m_CheckerboardTexture;
		Ref<Texture2D> m_SpriteSheet;
		Ref<SubTexture2D> m_TextureStairs;

		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

		uint32_t m_MapWidth, m_MapHeight;
		std::unordered_map<char, Ref<SubTexture2D>> m_TextureMap;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;

		int m_GizmoType = -1;

		//Panels;
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContextBrowserPanel;
	};
}
