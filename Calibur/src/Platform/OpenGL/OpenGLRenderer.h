#pragma once

#include "Calibur/Renderer/RendererAPI.h"

namespace Calibur
{
	class OpenGLRenderer: public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void SetDepthTest(bool flag) override;

		virtual void RenderMesh(Ref<Mesh> mesh, uint32_t submeshIndex) override;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
	};
}
