#pragma once

#ifdef HZ_PLATFORM_WINDOWS
	#ifdef HZ_BUILD_DLL
		#define HAZEL_API __declspec(dllexport)
	#else	
		#define HAZEL_API __declspec(dllimport)
	#endif
#else
	#error Hazel only support Windows!
#endif

#ifdef HZ_ENABLE_ASSERTS
	#define HZ_ASSRET(x, ...) { if (!(x)) {HZ_ERROR("Assertion Failed: {)}", __VA_ARGS__); __debuebreak();} }
	#define HZ_CORE_ASSRET(x, ...) { if (!(x)) {HZ_CORE_ERROR("Assertion Failed: {)}", __VA_ARGS__); __debuebreak();} }
#endif // HZ_ENABLE_ASSERTS
	#define HZ_ASSERT(x, ...)
	#define HZ_CORE_ASSERT(x, ...)

#define BIT(x) (1 << x)

#define HZ_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
