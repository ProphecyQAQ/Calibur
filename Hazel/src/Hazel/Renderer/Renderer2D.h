#pragma once
#include "Hazel/Renderer/OrthographicCamera.h"

#include <glm/glm.hpp>

namespace Hazel
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		// Primitives
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, glm::vec4& color);
	};
}
