#include "hzpch.h"
#include "Calibur/Renderer/RendererAPI.h"

#include "Platform/OpenGL/OpenGLRenderer.h"

namespace Calibur
{
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLRenderer>();
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
