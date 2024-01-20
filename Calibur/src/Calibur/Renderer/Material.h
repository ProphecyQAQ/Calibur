#pragma once

#include "Calibur/Renderer/Shader.h"
#include "Calibur/Renderer/Texture.h"

namespace Calibur
{
	struct MaterialUniforms
	{
		glm::vec4 Albedo  = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
		float Metallic    = 0.0f;
		float Roughness   = 0.0f;
		float Emission    = 0.0;
		uint32_t useNormalMap = 0;
	};

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
		virtual MaterialUniforms& GetMaterialUniforms() = 0;

		virtual void SetDiffuseMap(const Ref<Texture2D>& texture) = 0;
		virtual void SetNormalMap(const Ref<Texture2D>& texture) = 0;
		virtual void SetSpecMap(const Ref<Texture2D>& texture) = 0;
		virtual void SetRoughnessMap(const Ref<Texture2D>& texture) = 0;

		virtual Ref<Texture2D> GetDiffuseMap()  = 0;
		virtual Ref<Texture2D> GetNormalMap() = 0;
		virtual Ref<Texture2D> GetSpecMap() = 0;
		virtual Ref<Texture2D> GetRoughnessMap() = 0;
	};
}
