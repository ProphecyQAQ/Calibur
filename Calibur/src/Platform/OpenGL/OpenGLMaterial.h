#pragma once

#include "Calibur/Renderer/Texture.h"
#include "Calibur/Renderer/Material.h"

namespace Calibur
{
	class OpenGLMaterial : public Material
	{
	public:
		OpenGLMaterial(const Ref<Shader>& shader, const std::string& name);
		virtual ~OpenGLMaterial() = default;

		virtual void SetInt(const std::string& name, int value);
		virtual void SetFloat(const std::string& name, float value);
		virtual void SetFloat2(const std::string& name, const glm::vec2& value);
		virtual void SetFloat3(const std::string& name, const glm::vec3& value);
		virtual void SetFloat4(const std::string& name, const glm::vec4& value);
		virtual void SetMat4(const std::string& name, const glm::mat4& value);

		virtual Ref<Shader> GetShader() override { return m_Shader; };
		virtual const std::string& GetName() override { return m_Name; };
		virtual MaterialUniforms& GetMaterialUniforms() override { return m_MaterialUniforms; };

		virtual void SetDiffuseMap(const Ref<Texture2D>& texture) override { m_DiffuseMap = texture; };
		virtual void SetNormalMap(const Ref<Texture2D>& texture) override { m_NormalMap = texture; };
		virtual void SetSpecMap(const Ref<Texture2D>& texture) override { m_SpecMap = texture; };
		virtual void SetRoughnessMap(const Ref<Texture2D>& texture) override { m_RoughnessMap = texture; };

		virtual Ref<Texture2D> GetDiffuseMap() override { return m_DiffuseMap; };
		virtual Ref<Texture2D> GetNormalMap() override { return m_NormalMap; };
		virtual Ref<Texture2D> GetSpecMap() override { return m_SpecMap; };
		virtual Ref<Texture2D> GetRoughnessMap() override { return m_RoughnessMap; };
	private:
		std::string m_Name;

		Ref<Shader> m_Shader;
		MaterialUniforms m_MaterialUniforms;

		Ref<Texture2D> m_DiffuseMap, m_NormalMap, m_SpecMap, m_RoughnessMap;
	};
}
