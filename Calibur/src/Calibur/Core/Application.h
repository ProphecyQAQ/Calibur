#pragma once

#include "Calibur/Core/Base.h"

#include "Calibur/Core/Window.h"
#include "Calibur/Core/LayerStack.h"
#include "Calibur/Events/Event.h"
#include "Calibur/Events/ApplicationEvent.h"

#include "Calibur/Core/TimeStep.h"

#include "Calibur/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Calibur 
{
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			HZ_CORE_ASSERT(index < Count);
			return Args[index];
		}
	};

	class Application
	{
	public:
		Application(const std::string& name = "Calibur App", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
		virtual ~Application();


		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		
		static Application& Get() { return *s_Instance; }
		Window& GetWindow() { return *m_Window; }
		ApplicationCommandLineArgs GetCommandLineArgs() const { return m_CommandLineArgs; }

		void Close();
	private:
		void Run();
		bool OnWindowClosed(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		ApplicationCommandLineArgs m_CommandLineArgs;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		TimeStep m_TimeStep;
		float m_LastFrameTime = 0.0f; 
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};
	
	// To be defined in CLIENT
	Application* CreateApplication(ApplicationCommandLineArgs args);
}

