#pragma once

#include "Calibur/Renderer/Shader.h"
#include "Calibur/Renderer/Texture.h"

namespace Calibur
{
	class Material
	{
	public:
		static Ref<Material> Create(const Ref<Shader> shader, const std::string name = "Default");

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual Ref<Shader> GetShader() = 0;
		virtual const std::string& GetName() = 0;
	};
}
