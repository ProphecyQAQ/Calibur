#pragma once

#include "Calibur/Renderer/Camera.h"
#include "Calibur/Core/Timestep.h"
#include "Calibur/Events/Event.h"
#include "Calibur/Events/MouseEvent.h"

#include <glm/glm.hpp>

namespace Calibur
{
	enum class CameraMode
	{
		NONE, FLYCAM, ARCBALL
	};

	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void Init();

		void OnUpdate(TimeStep ts);
		void OnEvent(Event& e);

		float GetDistance() const { return m_Distance; }
		void SetDistance(float distance) { m_Distance = distance; }
		
		void SetViewportSize(float width, float height) 
		{ 
			if (m_ViewportWidth == width && m_ViewportHeight == height)
				return;
			m_ViewportWidth = width; m_ViewportHeight = height;  
			SetPrespectiveProjection(m_FOV, width, height, m_NearClip, m_FarClip);
			m_ViewportWidth = width;
			m_ViewportHeight = height;
		}

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

		glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		const glm::vec3& GetPosition() const { return m_Position; }
		glm::quat GetOrientation() const;
		float GetFov() { return m_FOV; };
		float GetNearClip() { return m_NearClip; };
		float GetFarClip() { return m_FarClip; };
		float GetAspectRatio() { return m_AspectRatio; }

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }
	private:
		void UpdateCameraView();

		bool OnMouseScroll(MouseScrolledEvent& e);

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition() const;

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;

	private:
		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

		glm::mat4 m_ViewMatrix;
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Direction;

		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

		float m_Distance = 10.0f;
		float m_NormalSpeed{ 0.002f };

		float m_Pitch = 0.0f, m_Yaw = 0.0f;
		float m_PitchDelta{}, m_YawDelta{};
		glm::vec3 m_PositionDelta{};
		
		float m_ViewportWidth = 1920, m_ViewportHeight = 1080;

		CameraMode m_CameraMode{ CameraMode::ARCBALL };

		constexpr static float MIN_SPEED{ 0.0005f }, MAX_SPEED{ 2.0f };
	};
}
