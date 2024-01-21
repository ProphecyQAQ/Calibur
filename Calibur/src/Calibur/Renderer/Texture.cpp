#include "hzpch.h"

#include "Calibur/Renderer/Texture.h"
#include "Calibur/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Calibur
{
	Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(specification);
		}
		
		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification, const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(specification, path);
		}
		
		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		
		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(const TextureSpecification& specification)
	{
		switch (Renderer::GetAPI()) {

		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTextureCube>(specification);
		}
		
		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		
		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(const TextureSpecification& specification, const std::string& directoryPath)
	{
		switch (Renderer::GetAPI()){
		
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTextureCube>(specification, directoryPath);
		}
		
		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		
		return nullptr;
	}
}
