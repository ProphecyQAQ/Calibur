#include "Sanbox2D.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

template<typename Fn>
class Timer
{
public:
	Timer(const char* name, Fn&& func)
		: m_Name(name), m_Stopped(false), m_Func(func)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if (!m_Stopped)
			Stop();
	}

	void Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		m_Stopped = true;

		float duration = (end - start) * 0.001f;

		//std::cout << m_Name << ": " << duration  << "ms" << std::endl;
		m_Func({ m_Name, duration });
	}
	
private:
	const char* m_Name;
	bool m_Stopped;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	Fn m_Func;
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name,  [&](ProfileResult profileResult) {m_ProfileResults.push_back(profileResult); })

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
	PROFILE_SCOPE("Sandbox2D::OnUpdate");

	// Update
	{
		PROFILE_SCOPE("CameraController::OnUpdate");
		m_CameraController.OnUpdate(ts);
	}

	// Render
	{
		PROFILE_SCOPE("Render Prep");
		Hazel::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		Hazel::RenderCommand::Clear();
	}

	{
		PROFILE_SCOPE("Render Draw");
		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

		Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 1.0f, 1.0f }, m_SquareColor);
		Hazel::Renderer2D::DrawQuad({ 0.0f, -0.0f, -0.9f }, { 0.5f, 0.5f }, m_CheckerboardTexture);
		Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, m_SquareColor);
	}

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

	for (auto& result : m_ProfileResults)
	{
		char label[50];
		strcat(label, "  %.3fms");
		strcpy(label, result.Name);
		ImGui::Text(label, result.Time);
	}
	m_ProfileResults.clear();

	ImGui::End();
}

void Sanbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}
