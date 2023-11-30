#pragma once

namespace Hazel
{
	enum class RendererAPI
	{
		None = 0,
		OpenGL,
		Direct3D,
	};
	
	class Renderer
	{
	public:
		inline static RendererAPI GetAPI() { return s_RendererAPI; }

		static RendererAPI s_RendererAPI;
	};
}
