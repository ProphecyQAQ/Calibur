#include "Sanbox2D.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name,  [&](ProfileResult profileResult) {m_ProfileResults.push_back(profileResult); })

Sanbox2D::Sanbox2D()
	:Layer("Sandbox2D"), m_CameraController(1920.0f/1080.0f)
{

}

void Sanbox2D::OnAttach()
{
	HZ_PROFILE_FUNCTION();

	m_CheckerboardTexture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sanbox2D::OnDetach()
{
	HZ_PROFILE_FUNCTION();
}

void Sanbox2D::OnUpdate(Hazel::TimeStep ts)
{
	HZ_PROFILE_FUNCTION();

	// Update
	m_CameraController.OnUpdate(ts);

	// Render
	{
		HZ_PROFILE_SCOPE("Render Prep");
		Hazel::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		Hazel::RenderCommand::Clear();
	}

	{
		HZ_PROFILE_SCOPE("Render Draw");
		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

		Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 1.0f, 1.0f }, m_SquareColor);
		Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, m_SquareColor);
		Hazel::Renderer2D::DrawQuad({ -5.0f, -5.0f, -0.1f }, { 10.0f, 10.0f } ,m_CheckerboardTexture, 10.0f, glm::vec4(1.0f, 0.9f, 0.9f, 1.0f));
		Hazel::Renderer2D::DrawQuad({ -0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f }, m_CheckerboardTexture, 20.0f);
		Hazel::Renderer2D::EndScene();
	}


}

void Sanbox2D::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sanbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}
