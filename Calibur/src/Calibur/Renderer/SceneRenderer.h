#pragma once

#include "Calibur/Scene/Scene.h"
#include "Calibur/Scene/Entity.h"
#include "Calibur/Renderer/Camera.h"
#include "Calibur/Renderer/UniformBuffer.h"

#include <glm/glm.hpp>

namespace Calibur
{
	struct SceneRenderCamera
	{
		Camera camera;
		glm::mat4 ViewMatrix;
		glm::vec3 position;
		float Fov;
		float Near, Far; //Non-reversed
		float AspectRatio;
	};

	class SceneRenderer
	{
	public:
		SceneRenderer(Ref<Scene> scene);
		~SceneRenderer() = default;

		void BeginScene(const SceneRenderCamera &camera);
		void EndScene();

		void SubmitLight(SceneLightData& lightData);

		struct CascadeData
		{
			glm::mat4 ViewProj;
			glm::mat4 View;
			float SplitDepth;
		};
		void GenerateShadowMap(const SceneLightData& lightData);
		void CalculateCascades(const glm::vec3& lightDirection);
		
		void TaaPass();

		void SetScene(Ref<Scene> scene) { m_Scene = scene; } 
		void SetFramebuffer(Ref<Framebuffer>& fbo) { m_ActiveFramebuffer = fbo; }

		Ref<UniformBuffer>& GetTransformUB() { return m_TransformBuffer; }
		Ref<UniformBuffer>& GetBoneMatricesUB() { return m_FinalBoneMatricesBuffer; }
	private:
		// Framebuffer will show the final result
		Ref<Framebuffer> m_ActiveFramebuffer;
		// Main framebuffer
		Ref<Framebuffer> m_MainFramebuffer;

		Ref<Scene> m_Scene;

		//Unfirom buffer
		Ref<UniformBuffer> m_CameraUniformBuffer;
		Ref<UniformBuffer> m_TransformBuffer;
		Ref<UniformBuffer> m_LightMatricesBuffer;
		Ref<UniformBuffer> m_TaaParamBuffer;
		Ref<UniformBuffer> m_FinalBoneMatricesBuffer;

		// Dir CSM data
		Ref<Framebuffer> m_CSMFramebuffer;
		Ref<Texture2DArray> m_CSMTextureArray;
		Ref<Shader> m_DirCSMShader;
		uint32_t m_DirCSMCount = 4;
		std::vector<CascadeData> m_CascadeData;
		std::vector<float> m_CascadeSplits;

		// Current scene data
		SceneRenderCamera m_SceneRenderCamera;

		// Previous framebuffer reslut
		Ref<Texture2D> m_PreviousFrame;
		Ref<Texture2D> m_CurrentFrame;
		Ref<Texture2D> m_MotionVertor;
		Ref<Texture2D> m_CurrentDepth;

	private:

		struct CameraUBData
		{
			glm::mat4 ViewProjectionMatrix;
			glm::mat4 ViewMatrix;
			glm::mat4 ProjectionMatrix;
			glm::vec4 CameraPosition;
		} CameraUB;

		struct DirectionalLightUBData
		{
			uint32_t Count = 0;
			glm::vec3 Padding{ 0.0 };
			DirectionalLight lights[4];
		} DirectionalLightUB;

		struct PointLightUBData
		{
			uint32_t Count = 0;
			glm::vec3 Padding{ 0.0 };
			PointLight lights[16];
		} PointLightUB;

		struct TaaParamUBData
		{
			glm::mat4 PreView;
			glm::mat4 PreProjection;
			float ScreenWidth;
			float ScreenHeight;
			float DeltaTime;
			uint32_t FrameIndex;
		} TaaParamUB;

		friend class EditorLayer;
	};
}
