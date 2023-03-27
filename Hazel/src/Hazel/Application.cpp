#include "Application.h"

#include "Hazel/Log.h"
#include "Hazel/Event/ApplicationEvent.h"

namespace Hazel {

	Application::Application() {

	}

	Application::~Application() {
			
	}

	void Application::Run() {
		WindowResizeEvent e(1280, 720);
		HZ_TRACE(e);

		while (true);
	}
}
