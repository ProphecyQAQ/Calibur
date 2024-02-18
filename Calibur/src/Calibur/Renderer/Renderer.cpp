#include "hzpch.h"

#include "Calibur/Renderer/Renderer.h"
#include "Calibur/Scene/Scene.h"

namespace Calibur
{
	static struct RenderData
	{
		Ref<ShaderLibrary> s_ShaderLibrary;
		Ref<Texture2D> s_WhiteTexture;

	}*s_RenderData;

	void Renderer::Init()
	{
		HZ_PROFILE_FUNCTION();

		RenderCommand::Init();
	
		s_RenderData = new RenderData();
		TextureSpecification spec;
		s_RenderData->s_WhiteTexture = Texture2D::Create(spec);
		uint32_t whiteTextureData = 0xffffffff;
		s_RenderData->s_WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		s_RenderData->s_ShaderLibrary = ShaderLibrary::Create();

		Renderer::GetShaderLibrary()->Load("./assets/shaders/Texture3D.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/textureToScreen.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/Skybox.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/Pbr.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/EquirectangularToCube.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/irradianceConvolution.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/prefilteredConvolution.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/BrdfLut.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/DirLightCSM.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/Pbr_withMotion.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/Taa.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/Terrain.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/PbrAnim_withMotion.glsl");
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

	void Renderer::BeginScene()
	{
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
