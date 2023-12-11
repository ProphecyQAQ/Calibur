#include "Sanbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

Sanbox2D::Sanbox2D()
	:Layer("Sandbox2D"), m_CameraController(1920.0f/1080.0f)
{

}

void Sanbox2D::OnAttach()
{
	m_SquareVA = Hazel::VertexArray::Create();

	float squareVertices[4 * 5] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
	};

	Hazel::Ref<Hazel::VertexBuffer> suqareVB(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
	
	Hazel::BufferLayout layout = {
		{Hazel::ShaderDataType::Float3, "a_Position"},
	};
	suqareVB->SetLayout(layout);
	m_SquareVA->AddVertexBuffer(suqareVB);

	unsigned int squareIndics[6] = { 0, 1, 2, 2, 3, 0 };
	Hazel::Ref<Hazel::IndexBuffer> squareIB(Hazel::IndexBuffer::Create(squareIndics, std::size(squareIndics)));

	m_SquareVA->SetIndexBuffer(squareIB);

	//Init shader
	m_FlatColorShader = Hazel::Shader::Create("assets/shaders/FlatColor.glsl");
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

	Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f));
	std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);
	
	Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, scale);

	//Triangle
	//Hazel::Renderer::Submit(m_Shader, m_VertexArray);

	Hazel::Renderer::EndScene();

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
