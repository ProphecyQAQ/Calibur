#include "hzpch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel
{
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	SceneCamera::~SceneCamera()
	{
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_AspectRatio = (float)width / (float)height;

		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float verticalFov, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;

		m_PerspectiveFOV = verticalFov;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;

		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographics;

		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar  = farClip;

		RecalculateProjection();  
	}

	void SceneCamera::RecalculateProjection()
	{
		if (m_ProjectionType == ProjectionType::Perspective)
		{
			m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}
		else if (m_ProjectionType == ProjectionType::Orthographics)
		{
			float orthoLeft   = -0.5f * m_AspectRatio * m_OrthographicSize;
			float orthoRight  =  0.5f * m_AspectRatio * m_OrthographicSize;
			float orthoTop    =  0.5f  * m_OrthographicSize;
			float orthoButtom = -0.5f  * m_OrthographicSize;

			m_Projection = glm::ortho(orthoLeft, orthoRight, 
				orthoTop, orthoButtom, m_OrthographicNear, m_OrthographicFar);
		}
	}
}
