#include "hzpch.h"

#include "Calibur/Renderer/Material.h"
#include "Calibur/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLMaterial.h"

namespace Calibur
{
	Ref<Material> Material::Create(const Ref<Shader> shader, const std::string name)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLMaterial>(shader, name);
		} 
		
		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");

		return nullptr;

	}
}
