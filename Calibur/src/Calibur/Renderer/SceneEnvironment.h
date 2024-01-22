#pragma once
#include "Calibur/Core/Base.h"
#include "Calibur/Renderer/Texture.h"
#include "Calibur/Renderer/Shader.h"
#include "Calibur/Renderer/Renderer.h"
#include "Calibur/Renderer/Framebuffer.h"

namespace Calibur
{
	class SceneEnvironment
	{
	public:
		SceneEnvironment(const std::string& path, bool isVerticalFlip = false);
		~SceneEnvironment() = default;

		static Ref<Texture2D> GetBrdfLutMap() { return s_BrdfLut; }
 		Ref<Texture2D> GetEquirectangularMap() { return m_EquirectangularMap; }
		Ref<TextureCube> GetPreFilterMap() { return m_PrefilterMap; }
		Ref<TextureCube> GetIrradianceMap() { return m_IrradianceMap; }
		Ref<TextureCube> GetSkybox() { return m_Skybox; }

	private:
		void DrawEquirectangularToCubemap(Ref<Texture2D> equirectangularMap);
		void DrawIrradianceMap();
		void DrawPrefilterMap();
		static void DrawBrdfLut();

	private:
		Ref<TextureCube> m_Skybox;
		Ref<TextureCube> m_IrradianceMap;
		Ref<TextureCube> m_PrefilterMap;
		Ref<Texture2D> m_EquirectangularMap;
		static Ref<Texture2D> s_BrdfLut;

		Ref<Shader> m_EquirectangularToCubemapShader;
		Ref<Shader> m_SkyboxShader;
		Ref<Shader> m_PrefilterShader;
		Ref<Shader> m_IrradianceShader;

		Ref<Framebuffer> m_CaptureFramebuffer;
		FramebufferSpecification m_FboSpec;
	};
}
