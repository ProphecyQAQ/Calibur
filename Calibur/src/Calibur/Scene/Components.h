#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Calibur/Scene/SceneCamera.h"
#include "Calibur/Scene/ScriptableEntity.h"
#include "Calibur/Renderer/Mesh.h"
#include "Calibur/Renderer/Material.h"

namespace Calibur
{
	
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};
	
	struct TransformComponent
	{
		glm::vec3 Translation = {0.0f, 0.0f, 0.0f};
		glm::vec3 Rotation    = {0.0f, 0.0f, 0.0f};
		glm::vec3 Scale      = {1.0f, 1.0f, 1.0f};

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}
		
		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation) *
				rotation * glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;
		
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)();
		void(*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

	struct MeshComponent
	{
		Ref<Mesh> mesh;
		Ref<Material> material;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const std::string& filepath, bool isVerticalFlip)
			: mesh(std::make_shared<Mesh>(filepath, isVerticalFlip)) {}
		MeshComponent(const Ref<Mesh>& mesh)
			: mesh(mesh) {}
	};
}
