#include <Hazel.h>
#include "Hazel/Core/EntryPoint.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>

#include "Platform/OpenGL/OpenGLShader.h"
#include "Sanbox2D.h"

class ExampleLayer : public Hazel::Layer 
{
public:
	ExampleLayer() 
		: Layer("Example"), m_CameraController(1920.f/1080.f, true)
	{
		m_VertexArray = Hazel::VertexArray::Create();

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		 	 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		};

		m_VertexBuffer = Hazel::VertexBuffer::Create(vertices, sizeof(vertices));

		Hazel::BufferLayout layout = {
			{Hazel::ShaderDataType::Float3, "a_Position"},
			{Hazel::ShaderDataType::Float2, "a_TexCoord"},
		};

		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		 
		unsigned int indics[3] = { 0, 1, 2 };

		m_IndexBuffer = Hazel::IndexBuffer::Create(indics, std::size(indics));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		m_SquareVA = Hazel::VertexArray::Create();
		float squareVertices[4 * 5] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 	 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		};
		Hazel::Ref<Hazel::VertexBuffer> suqareVB = Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices));

		suqareVB->SetLayout(layout);
		m_SquareVA->AddVertexBuffer(suqareVB);

		unsigned int squareIndics[6] = { 0, 1, 2, 2, 3, 0 };
		Hazel::Ref<Hazel::IndexBuffer> squareIB = Hazel::IndexBuffer::Create(squareIndics, std::size(squareIndics));

		m_SquareVA->SetIndexBuffer(squareIB);


		std::string vertexSrc = R"(
			#version 330 core
				
			layout(location = 0) in vec3 a_Position;			

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec4 fragPos;

			void main()
			{
				fragPos = vec4(a_Position, 1.0);
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}	
		)";
		
		std::string fragmentSrc = R"(
			#version 330 core
			
			uniform vec3 u_Color;

			in vec4 fragPos;
			layout(location = 0) out vec4 color;		

			void main()
			{
				color = vec4(u_Color, 1.0);	
			}	
		)";

		//Init shader
		m_Shader = Hazel::Shader::Create("flatColorShader", vertexSrc, fragmentSrc);

		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
		m_ChernoTexture = Hazel::Texture2D::Create("assets/textures/ChernoLogo.png");

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(Hazel::TimeStep ts) override
	{
		// Update
		m_CameraController.OnUpdate(ts);  

		//Render
		Hazel::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		Hazel::RenderCommand::Clear();

		Hazel::Renderer::BeginScene(m_CameraController.GetCamera());
		
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		glm::vec4 redColor(0.8, 0.2, 0.3, 1.0);
		glm::vec4 blueColor(0.2, 0.3, 0.8, 1.0);

		for (int y = 0; y < 20; y++) {
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 position(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * scale;

				std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", m_SquareColor);

				Hazel::Renderer::Submit(m_Shader, m_SquareVA, transform);
			}
		}
		
		auto textureShader = m_ShaderLibrary.Get("Texture");

		m_Texture->Bind();
		Hazel::Renderer::Submit(textureShader, m_SquareVA);
		m_ChernoTexture->Bind();
		Hazel::Renderer::Submit(textureShader, m_SquareVA);
		//Triangle
		//Hazel::Renderer::Submit(m_Shader, m_VertexArray);

		Hazel::Renderer::EndScene();
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(Hazel::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}

private:
	Hazel::ShaderLibrary m_ShaderLibrary;
	Hazel::Ref<Hazel::Shader> m_Shader;
	Hazel::Ref<Hazel::Texture2D> m_Texture, m_ChernoTexture;

	Hazel::Ref<Hazel::VertexBuffer> m_VertexBuffer;
	Hazel::Ref<Hazel::IndexBuffer> m_IndexBuffer;
	Hazel::Ref<Hazel::VertexArray> m_VertexArray;
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;

	Hazel::OrthographicCameraController m_CameraController;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f};
};

class Sandbox : public Hazel::Application 
{
public:
	Sandbox() 
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sanbox2D());
	}

	~Sandbox() {


	}

};

Hazel::Application* Hazel::CreateApplication() {
	return new Sandbox();
}
