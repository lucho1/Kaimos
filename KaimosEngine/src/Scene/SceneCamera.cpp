#include "kspch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Kaimos {

	// ----------------------- Public Class Methods ------------------------------------------------------
	SceneCamera::SceneCamera()
	{
		RecalculateProjectionMatrix();
	}



	// ----------------------- Public Camera Methods ----------------------------------------------------
	void SceneCamera::SetViewportSize(uint width, uint height)
	{
		if (height == 0)
			return;

		m_AspectRatio = (float)width / (float)height;
		RecalculateProjectionMatrix();
	}



	// ----------------------- Orthographic Camera G/Setters ---------------------------------------------
	void SceneCamera::SetOrthographicCamera(float size, float near_clip, float far_clip)
	{
		m_ProjectionType = PROJECTION_TYPE::ORTHOGRAPHIC;

		m_OrthoSize = size;
		m_OrthographicNearClip = near_clip;
		m_OrthographicFarClip = far_clip;

		RecalculateProjectionMatrix();
	}



	// ----------------------- Perspective Camera G/Setters ----------------------------------------------
	void SceneCamera::SetPerspectiveCamera(float FOV, float near_clip, float far_clip)
	{
		m_ProjectionType = PROJECTION_TYPE::PERSPECTIVE;

		m_PerspectiveFOV = FOV;
		m_PerspectiveNearClip = near_clip;
		m_PerspectiveFarClip = far_clip;

		RecalculateProjectionMatrix();
	}

	

	// ----------------------- Private Camera Methods ----------------------------------------------------
	void SceneCamera::RecalculateProjectionMatrix()
	{
		if (m_ProjectionType == PROJECTION_TYPE::PERSPECTIVE)
			m_ProjectionMatrix = glm::perspective(glm::radians(m_PerspectiveFOV), m_AspectRatio, m_PerspectiveNearClip, m_PerspectiveFarClip);
		else
		{
			float right = m_OrthoSize * m_AspectRatio * 0.5f;
			float top = m_OrthoSize * 0.5f;
			float left = -right;
			float bot = -top;

			m_ProjectionMatrix = glm::ortho(left, right, bot, top, m_OrthographicNearClip, m_OrthographicFarClip);
		}
	}
}
