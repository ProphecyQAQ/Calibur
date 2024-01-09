#pragma once

#include "Shader.h"
#include "Calibur/Renderer/RenderCommand.h"
#include "Calibur/Renderer/UniformBuffer.h"

#include "Calibur/Renderer/OrthographicCamera.h"
#include "Calibur/Renderer/EditorCamera.h"
#include "Calibur/Renderer/Shader.h"

#include <glm/glm.hpp>

namespace Calibur
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(OrthographicCamera& camera);
		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(EditorCamera& camera);
		static void EndScene();

		static void Submit(
			const Ref<Shader>& shader, 
			const Ref<VertexArray>& vertexArray,
			const glm::mat4& transform = glm::mat4(1.0f));

		static void RenderMesh(Ref<Mesh> mesh, uint32_t submeshIndex);

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		static Ref<ShaderLibrary> GetShaderLibrary();
		static Ref<Texture2D> GetWhiteTexture();
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static Scope<SceneData> m_SceneData;
	};
}
