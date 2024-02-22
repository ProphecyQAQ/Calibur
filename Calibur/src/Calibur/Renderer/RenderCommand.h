#pragma once

#include "Calibur/Renderer/RendererAPI.h"

namespace Calibur
{
	class RenderCommand
	{
	public:
		static void Init()
		{
			s_RendererAPI->Init();
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		static void SetClearColor(const glm::vec4& color) 
		{ 
			s_RendererAPI->SetClearColor(color); 
		};

		static void SetDepthTest(bool flag)
		{
			s_RendererAPI->SetDepthTest(flag);
		}

		static void SetFaceCulling(bool flag, int type = 0)
		{
			s_RendererAPI->SetFaceCulling(flag, type);
		}

		static void Clear()
		{ 
			s_RendererAPI->Clear(); 
		};

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
			s_DrawCallsCount++;
		}

		static void DrawPatches(const Ref<VertexArray>& vertexArray, uint32_t size)
		{
			s_RendererAPI->DrawPatches(vertexArray, size);
			s_DrawCallsCount++;
		}

		static void RenderMesh(Ref<Mesh> mesh, uint32_t submeshIndex)
		{
			s_RendererAPI->RenderMesh(mesh, submeshIndex);
			s_DrawCallsCount++;
		}

		static int GetDrawCallsCount()
		{
			return s_DrawCallsCount;
		}

		static void ClearDrawCallsCount()
		{
			s_DrawCallsCount = 0;
		}

	private:
		static Scope<RendererAPI> s_RendererAPI;
		static int s_DrawCallsCount;
	};
}
