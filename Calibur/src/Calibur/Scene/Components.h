#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Calibur/Core/UUID.h"
#include "Calibur/Math/Math.h"
#include "Calibur/Scene/SceneCamera.h"
#include "Calibur/Renderer/Mesh.h"
#include "Calibur/Renderer/Texture.h"
#include "Calibur/Renderer/Material.h"

namespace Calibur
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct RelationshipComponent
	{
		UUID ParentID = 0;
		std::vector<UUID> Children;

		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent& other) = default;
		RelationshipComponent(UUID parent)
			: ParentID(parent) {}
	};
	
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
		void SetTransform(glm::mat4& transform)
		{
			Math::DecomposeTransform(transform, Translation, Rotation, Scale);
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;

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
	
	class ScriptableEntity;
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
		std::vector<uint32_t> SubmeshIndices;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const std::string& filepath, bool isVerticalFlip)
			: mesh(std::make_shared<Mesh>(filepath, isVerticalFlip)) {}
		MeshComponent(Ref<Mesh> mesh)
			: mesh(mesh) {}
	};

	enum class LightType
	{
		None = 0, Directional, Point, Spot
	};

	struct DirectionalLightComponent
	{
		glm::vec3 Radiance{1.0f};
		float Intensity = 1.0f;
	};

	struct PointLightComponent
	{
		glm::vec3 Radiance{1.0f};
		float Intensity = 0.0f;
		float Radius = 25.0f;
		float SourceSize = 0.1f;
		bool CastShadow = 0;
	};
}
