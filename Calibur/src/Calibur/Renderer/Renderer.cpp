#include "hzpch.h"

#include "Calibur/Renderer/Renderer.h"

namespace Calibur
{
	Scope<Renderer::SceneData> Renderer::m_SceneData = CreateScope<Renderer::SceneData>();
	static Ref<UniformBuffer> m_CameraUniformBuffer;

	static struct RenderData
	{
		Ref<ShaderLibrary> s_ShaderLibrary;
		Ref<Texture2D> s_WhiteTexture;
	}*s_RenderData;

	void Renderer::Init()
	{
		HZ_PROFILE_FUNCTION();

		RenderCommand::Init();

		m_CameraUniformBuffer = UniformBuffer::Create(sizeof(SceneData), 0);
		
		s_RenderData = new RenderData();
		s_RenderData->s_WhiteTexture = Texture2D::Create(1, 1);
		s_RenderData->s_ShaderLibrary = ShaderLibrary::Create();
		Renderer::GetShaderLibrary()->Load("./assets/shaders/Texture3D.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/textureToScreen.glsl");
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

	void Renderer::RenderMesh(Ref<Mesh> mesh, uint32_t submeshIndex)
	{
		mesh->GetVertexArray()->Bind();
		RenderCommand::RenderMesh(mesh, submeshIndex);
	}
	
	Ref<ShaderLibrary> Renderer::GetShaderLibrary()
	{
		return s_RenderData->s_ShaderLibrary;
	}

	Ref<Texture2D> Renderer::GetWhiteTexture()
	{
		return s_RenderData->s_WhiteTexture;
	}
}