#pragma once

#include "hzpch.h"

#include "Calibur/Core/Base.h"
#include "Calibur/Events/Event.h"

namespace Calibur 
{
	struct WindowProps 
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(
			const std::string& title = "Calibur Editor",
			uint32_t width = 1920,
			uint32_t height = 1080
		) : Title(title), Width(width), Height(height)
		{

		}
	};
	
	// Interface representing a desktop system based Window
	class Window 
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
	
		//Window attrivutes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enable) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Scope<Window> Create(const WindowProps& Props = WindowProps());
	};
}
