#include "hzpch.h"

#include "Calibur/Renderer/Renderer.h"
#include "Calibur/Scene/Scene.h"

namespace Calibur
{
	static Ref<UniformBuffer> s_CameraUniformBuffer;
	static Ref<UniformBuffer> s_DirectionalLightUniformBuffer;

	static struct RenderData
	{
		Ref<ShaderLibrary> s_ShaderLibrary;
		Ref<Texture2D> s_WhiteTexture;

	}*s_RenderData;

	void Renderer::Init()
	{
		HZ_PROFILE_FUNCTION();

		RenderCommand::Init();

		s_DirectionalLightUniformBuffer = UniformBuffer::Create(sizeof(SceneLightData), 3);
		
		s_RenderData = new RenderData();
		s_RenderData->s_WhiteTexture = Texture2D::Create(1, 1);
		s_RenderData->s_ShaderLibrary = ShaderLibrary::Create();

		Renderer::GetShaderLibrary()->Load("./assets/shaders/Texture3D.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/textureToScreen.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/Skybox.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/Pbr.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/EquirectangularToCube.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/irradianceConvolution.glsl");
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
	}

	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
	}

	void Renderer::BeginScene(EditorCamera& camera)
	{
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::SubmitLight(SceneLightData& light)
	{
		s_DirectionalLightUniformBuffer->SetData(&light.DirectionalLights, sizeof(SceneLightData));
	}

	void Renderer::Submit(
		const Ref<Shader>& shader, 
		const Ref<VertexArray>& vertexArray,
		const glm::mat4& transform)
	{
		shader->Bind();

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

	void Renderer::RenderMesh(Ref<Mesh> mesh, uint32_t submeshIndex)
	{
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
