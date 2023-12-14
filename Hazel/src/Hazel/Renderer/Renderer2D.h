#pragma once
#include "Hazel/Renderer/OrthographicCamera.h"
#include "Hazel/Renderer/Texture.h"

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

		static void Flush();

		// Primitives
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, Ref<Texture2D>& texture, float tillingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, Ref<Texture2D>& texture, float tillingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawRotateQuad(const glm::vec2& position, const glm::vec2& size, float rotate, glm::vec4& color);
		static void DrawRotateQuad(const glm::vec3& position, const glm::vec2& size, float rotate, glm::vec4& color);
		static void DrawRotateQuad(const glm::vec2& position, const glm::vec2& size, float rotate, Ref<Texture2D>& texture, float tillingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotateQuad(const glm::vec3& position, const glm::vec2& size, float rotate, Ref<Texture2D>& texture, float tillingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
	};
}
