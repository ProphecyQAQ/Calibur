#include "hzpch.h"

#include "Calibur/Renderer/VertexArray.h"
#include "Calibur/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Calibur
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexArray>();
		} 
		
		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");

		return nullptr;
	}

}

