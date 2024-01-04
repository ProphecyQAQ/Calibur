#pragma once

#include "Calibur/Core/Base.h"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Calibur {
	class Log
	{
	public:
		static void Init();

		static Ref<spdlog::logger>& GetCoreLogger() {
			return s_CoreLogger;
		}
		static Ref<spdlog::logger>& GetClientLogger() {
			return s_ClientLogger;
		}

	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};
}

// Core Log macros
#define HZ_CORE_CRITICAL(...) ::Calibur::Log::GetCoreLogger()->critical(__VA_ARGS__)
#define HZ_CORE_ERROR(...)    ::Calibur::Log::GetCoreLogger()->error(__VA_ARGS__)
#define HZ_CORE_WARN(...)     ::Calibur::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define HZ_CORE_INFO(...)     ::Calibur::Log::GetCoreLogger()->info(__VA_ARGS__)
#define HZ_CORE_TRACE(...)    ::Calibur::Log::GetCoreLogger()->trace(__VA_ARGS__)

// CLient log macros
#define HZ_CRITICAL(...)      ::Calibur::Log::GetClientLogger()->critical(__VA_ARGS__)
#define HZ_ERROR(...)         ::Calibur::Log::GetClientLogger()->error(__VA_ARGS__)
#define HZ_WARN(...)          ::Calibur::Log::GetClientLogger()->warn(__VA_ARGS__)
#define HZ_INFO(...)          ::Calibur::Log::GetClientLogger()->info(__VA_ARGS__)
#define HZ_TRACE(...)		  ::Calibur::Log::GetClientLogger()->trace(__VA_ARGS__)
