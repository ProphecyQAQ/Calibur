#pragma once

#include "Calibur/Renderer/Texture.h"
#include "Calibur/Renderer/Shader.h"
#include "Calibur/Renderer/VertexArray.h"

namespace Calibur
{
	class Terrain
	{
	public:
		Terrain(Ref<Texture2D> heightMap);
		~Terrain() = default;

		void Render();

	private:
		Ref<Texture2D> m_HeightMap;
		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<Shader> m_Shader;

		uint32_t m_Size;
		friend class SceneRenderer;
	};
}
