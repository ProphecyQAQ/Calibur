#include <Hazel.h>

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
			{Hazel::ShaderDataType::Float4, "a_Color"},
		};

		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		 
		unsigned int indics[3] = { 0, 1, 2 };

		m_IndexBuffer.reset(Hazel::IndexBuffer::Create(indics, std::size(indics)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		m_SquareVA.reset(Hazel::VertexArray::Create());
		float squareVertices[4 * 7] = {
			-0.75f, -0.75f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		 	 0.75f, -0.75f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			 0.75f,  0.75f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
			-0.75f,  0.75f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		};
		std::shared_ptr<Hazel::VertexBuffer> suqareVB(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		suqareVB->SetLayout(layout);
		m_SquareVA->AddVertexBuffer(suqareVB);

		unsigned int squareIndics[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Hazel::IndexBuffer> squareIB(Hazel::IndexBuffer::Create(squareIndics, std::size(squareIndics)));

		m_SquareVA->SetIndexBuffer(squareIB);


		std::string vertexSrc = R"(
			#version 330 core
				
			layout(location = 0) in vec3 a_Position;			
			layout(location = 1) in vec4 a_Color;			

			uniform mat4 u_ViewProjection;

			out vec4 fragPos;
			out vec4 fragColor;

			void main()
			{
				fragPos = vec4(a_Position, 1.0);
				fragColor = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}	
		)";
		
		std::string fragmentSrc = R"(
			#version 330 core
			
			in vec4 fragPos;
			in vec4 fragColor;
			layout(location = 0) out vec4 color;			

			void main()
			{
				color = fragColor;	
			}	
		)";

		//Init shader
		m_Shader.reset(new Hazel::Shader(vertexSrc, fragmentSrc));
	}

	void OnUpdate() override
	{
		if (Hazel::Input::IsKeyPressed(HZ_KEY_LEFT))
		{
			m_CameraPosition.x += m_CameraMoveSpeed;
		}
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT))
		{
			m_CameraPosition.x -= m_CameraMoveSpeed;
		}
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_UP))
		{
			m_CameraPosition.y -= m_CameraMoveSpeed;
		}
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_DOWN))
		{
			m_CameraPosition.y += m_CameraMoveSpeed;
		}
		
		if (Hazel::Input::IsKeyPressed(HZ_KEY_A))
			m_CameraRotation += m_CameraRotationSpeed;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_D))
			m_CameraRotation -= m_CameraRotationSpeed;


		Hazel::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		Hazel::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Hazel::Renderer::BeginScene(m_Camera);

		Hazel::Renderer::Submit(m_Shader, m_SquareVA);
		Hazel::Renderer::Submit(m_Shader, m_VertexArray);

		Hazel::Renderer::EndScene();
	}

	void OnImGuiRender() override
	{
	}

	void OnEvent(Hazel::Event& event) override
	{
	}

private:

	std::shared_ptr<Hazel::Shader> m_Shader;
	std::shared_ptr<Hazel::VertexBuffer> m_VertexBuffer;
	std::shared_ptr<Hazel::IndexBuffer> m_IndexBuffer;
	std::shared_ptr<Hazel::VertexArray> m_VertexArray;

	std::shared_ptr<Hazel::VertexArray> m_SquareVA;

	Hazel::OrthographicCamera m_Camera;

	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 0.01f;

	float m_CameraRotation = 0.f;
	float m_CameraRotationSpeed = 1.0f;
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
