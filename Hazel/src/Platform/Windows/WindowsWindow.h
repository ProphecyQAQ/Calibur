#pragma once

#include "Hazel/Core/Window.h"
#include "Hazel/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Hazel
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;
		
		unsigned int GetWidth() const { return m_Data.Width; };
		unsigned int GetHeight() const { return m_Data.Height; };
		
		//Window attrivutes
		void SetEventCallback(const EventCallbackFn& callback) override 
		{
			m_Data.EventCallback = callback;
		}
		void SetVSync(bool enable) override;
		bool IsVSync() const override;

		virtual void* GetNativeWindow() const { return m_Window; };

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title;
			unsigned int Width;
			unsigned int Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
		Scope<GraphicsContext> m_Context;
	};
}
