#include "hzpch.h"

#include "Calibur/Renderer/Texture.h"
#include "Calibur/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Calibur
{
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(width, height);
		}
		
		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path, bool isVerticalFlip)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(path, isVerticalFlip);
		}
		
		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		
		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI()){
		
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTextureCube>(width, height);
		}
		
		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		
		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(const std::string& directoryPath, bool isVerticalFlip)
	{
		switch (Renderer::GetAPI()){
		
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTextureCube>(directoryPath, isVerticalFlip);
		}
		
		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		
		return nullptr;
	}
}
