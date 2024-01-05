#include "hzpch.h"

#include "Calibur/Renderer/Renderer.h"

namespace Calibur
{
	Scope<Renderer::SceneData> Renderer::m_SceneData = CreateScope<Renderer::SceneData>();
	static Ref<UniformBuffer> m_CameraUniformBuffer;

	void Renderer::Init()
	{
		HZ_PROFILE_FUNCTION();

		RenderCommand::Init();

		m_CameraUniformBuffer = UniformBuffer::Create(sizeof(SceneData), 0);
	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::BeginScene(EditorCamera& camera)
	{
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjection();
		m_CameraUniformBuffer->SetData(&m_SceneData->ViewProjectionMatrix, sizeof(SceneData));
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(
		const Ref<Shader>& shader, 
		const Ref<VertexArray>& vertexArray,
		const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		shader->SetMat4("u_Transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}
