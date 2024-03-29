#pragma once

#include "Calibur/Renderer/VertexArray.h"
#include "Calibur/Renderer/Mesh.h"

#include <glm/glm.hpp> 

namespace Calibur
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};

	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void SetDepthTest(bool flag) = 0;
		virtual void SetFaceCulling(bool flag, int type = 0) = 0;
		
		virtual void RenderMesh(Ref<Mesh> mesh, uint32_t submeshIndex) = 0;
		virtual void DrawPatches(const Ref<VertexArray>& vertexArray, uint32_t size) = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		static API GetAPI() { return s_API; }

		static Scope<RendererAPI> Create();
	private:
		static API s_API;
	};
}
