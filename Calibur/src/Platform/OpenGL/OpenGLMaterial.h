#pragma once

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

		virtual Ref<Shader> GetShader() { return m_Shader; };
		virtual const std::string& GetName() { return m_Name; };
	private:
		Ref<Shader> m_Shader;
		std::string m_Name;
	};
}
