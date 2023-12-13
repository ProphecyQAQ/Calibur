#include "Sanbox2D.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sanbox2D::Sanbox2D()
	:Layer("Sandbox2D"), m_CameraController(1920.0f/1080.0f)
{

}

void Sanbox2D::OnAttach()
{
	m_CheckerboardTexture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sanbox2D::OnDetach()
{
}

void Sanbox2D::OnUpdate(Hazel::TimeStep ts)
{
	// Update
	m_CameraController.OnUpdate(ts);

	// Render
	Hazel::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
	Hazel::RenderCommand::Clear();

	Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

	Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 1.0f, 1.0f }, m_SquareColor);
	Hazel::Renderer2D::DrawQuad({ 0.0f, -0.0f, -0.9f}, { 0.5f, 0.5f }, m_CheckerboardTexture);
	Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, m_SquareColor);

	//glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f));
	//std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();
	//std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);
	
	//Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, scale);

	//Triangle
	//Hazel::Renderer::Submit(m_Shader, m_VertexArray);

	Hazel::Renderer2D::EndScene();

}

void Sanbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sanbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}
