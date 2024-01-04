#pragma once

#include "Calibur/Core/Base.h"

#ifdef HZ_PLATFORM_WINDOWS

extern Calibur::Application* Calibur::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv) 
{
	Calibur::Log::Init();
	HZ_PROFILE_BEGIN_SESSION("Startup", "CaliburProfile-Startup.json");
	auto app = Calibur::CreateApplication({argc, argv});
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Runtime", "CaliburProfile-Runtime.json");
	app->Run();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Shutdown", "CaliburProfile-Shutdown.json");
	delete app;
	HZ_PROFILE_END_SESSION();
}

#endif
