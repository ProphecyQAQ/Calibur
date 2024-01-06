#include "hzpch.h"
#include "OpenGLMaterial.h"

namespace Calibur
{
	OpenGLMaterial::OpenGLMaterial(const Ref<Shader>& shader, const std::string& name)
		: m_Shader(shader), m_Name(name)
	{
	}

	void OpenGLMaterial::SetInt(const std::string& name, int value)
	{
	}

	void OpenGLMaterial::SetFloat(const std::string& name, float value)
	{
	}

	void OpenGLMaterial::SetFloat2(const std::string& name, const glm::vec2& value)
	{
	}

	void OpenGLMaterial::SetFloat3(const std::string& name, const glm::vec3& value)
	{
	}

	void OpenGLMaterial::SetFloat4(const std::string& name, const glm::vec4& value)
	{
	}

	void OpenGLMaterial::SetMat4(const std::string& name, const glm::mat4& value)
	{
	}
}
