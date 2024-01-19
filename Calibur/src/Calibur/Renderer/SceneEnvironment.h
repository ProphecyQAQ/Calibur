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

		void DrawEquirectangularToCubemap(Ref<Texture2D> equirectangularMap);
		void DrawIrradianceMap();
		Ref<Texture2D> GetEquirectangularMap() { return m_EquirectangularMap; }
		Ref<TextureCube> GetRadianceMap() { return m_RadianceMap; }
		Ref<TextureCube> GetIrradianceMap() { return m_IrradianceMap; }
		Ref<TextureCube> GetSkybox() { return m_Skybox; }

	private:
		Ref<TextureCube> m_Skybox;
		Ref<TextureCube> m_RadianceMap;
		Ref<TextureCube> m_IrradianceMap;
		Ref<Texture2D> m_EquirectangularMap;

		Ref<Shader> m_EquirectangularToCubemapShader;
		Ref<Shader> m_SkyboxShader;
		Ref<Shader> m_RadianceShader;
		Ref<Shader> m_IrradianceShader;

		Ref<Framebuffer> m_CaptureFramebuffer;
	};
}
