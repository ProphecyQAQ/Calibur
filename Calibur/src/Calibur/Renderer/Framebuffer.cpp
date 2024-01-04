#include "hzpch.h"
#include "Calibur/Renderer/Framebuffer.h"
#include "Calibur/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Calibur
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLFramebuffer>(spec);
		}
		
		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}