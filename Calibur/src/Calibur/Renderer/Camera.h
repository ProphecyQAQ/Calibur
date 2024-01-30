#pragma once

#include <glm/glm.hpp>

namespace Calibur
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			: m_Projection(projection) {}

		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_Projection; }
		void SetProjection(const glm::mat4 projection) { m_Projection = projection; }
		void SetPrespectiveProjection(float verticalFov, float width, float height, float nearClip, float farClip)
		{
			m_Projection = glm::perspectiveFov(verticalFov, width, height, nearClip, farClip);
		}
	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};
}
