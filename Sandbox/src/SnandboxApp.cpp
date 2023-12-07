#include <Hazel.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"

#include "Platform/OpenGL/OpenGLShader.h"

class ExampleLayer : public Hazel::Layer 
{
public:
	ExampleLayer() 
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f ), m_CameraPosition(m_Camera.GetPosition())
	{
		m_VertexArray.reset(Hazel::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		 	 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		};

		m_VertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));

		Hazel::BufferLayout layout = {
			{Hazel::ShaderDataType::Float3, "a_Position"},
			{Hazel::ShaderDataType::Float2, "a_TexCoord"},
		};

		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		 
		unsigned int indics[3] = { 0, 1, 2 };

		m_IndexBuffer.reset(Hazel::IndexBuffer::Create(indics, std::size(indics)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		m_SquareVA.reset(Hazel::VertexArray::Create());
		float squareVertices[4 * 5] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 	 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		};
		Hazel::Ref<Hazel::VertexBuffer> suqareVB(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		suqareVB->SetLayout(layout);
		m_SquareVA->AddVertexBuffer(suqareVB);

		unsigned int squareIndics[6] = { 0, 1, 2, 2, 3, 0 };
		Hazel::Ref<Hazel::IndexBuffer> squareIB(Hazel::IndexBuffer::Create(squareIndics, std::size(squareIndics)));

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
		m_Shader.reset(Hazel::Shader::Create(vertexSrc, fragmentSrc));

		std::string textureVertexSrc = R"(
			#version 330 core
				
			layout(location = 0) in vec3 a_Position;			
			layout(location = 1) in vec2 a_TexCoord;			

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec2 v_TexCoord;

			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}	
		)";

		std::string textureFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;		

			in vec2 v_TexCoord;
			uniform sampler2D u_Texture;

			void main()
			{
				color = texture(u_Texture, v_TexCoord);	
			}	
		)";

		m_TextureShader.reset(Hazel::Shader::Create(textureVertexSrc, textureFragmentSrc));

		m_Texture = Hazel::Texture2D::Create("assets/textures/CheckerBoard.png");
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_TextureShader)->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(Hazel::TimeStep ts) override
	{
		float time = ts;

		if (Hazel::Input::IsKeyPressed(HZ_KEY_LEFT))
		{
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		}
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT))
		{
			m_CameraPosition.x += m_CameraMoveSpeed * ts;
		}

		if (Hazel::Input::IsKeyPressed(HZ_KEY_UP))
		{
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		}
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_DOWN))
		{
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;
		}
		
		if (Hazel::Input::IsKeyPressed(HZ_KEY_A))
			m_CameraRotation += m_CameraRotationSpeed * ts;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_D))
			m_CameraRotation -= m_CameraRotationSpeed * ts;

		Hazel::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		Hazel::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Hazel::Renderer::BeginScene(m_Camera);
		
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

		m_Texture->Bind();
		Hazel::Renderer::Submit(m_TextureShader, m_SquareVA);
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

	void OnEvent(Hazel::Event& event) override
	{
	}

private:

	Hazel::Ref<Hazel::Shader> m_Shader, m_TextureShader;
	Hazel::Ref<Hazel::Texture2D> m_Texture;

	Hazel::Ref<Hazel::VertexBuffer> m_VertexBuffer;
	Hazel::Ref<Hazel::IndexBuffer> m_IndexBuffer;
	Hazel::Ref<Hazel::VertexArray> m_VertexArray;
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;

	Hazel::OrthographicCamera m_Camera;

	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 1.f;

	float m_CameraRotation = 0.f;
	float m_CameraRotationSpeed = 10.f;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f};
};

class Sandbox : public Hazel::Application 
{
public:
	Sandbox() 
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {


	}

};

Hazel::Application* Hazel::CreateApplication() {
	return new Sandbox();
}
