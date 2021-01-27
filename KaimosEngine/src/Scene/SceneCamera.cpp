#include "kspch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Kaimos {

	SceneCamera::SceneCamera()
	{
		RecalculateProjectionMatrix();
	}

	void SceneCamera::SetOrhographicCamera(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::ORTHOGRAPHIC;

		m_OrthoSize = size;
		m_OrthographicNearClip = nearClip;
		m_OrthographicFarClip = farClip;

		RecalculateProjectionMatrix();
	}

	void SceneCamera::SetPerspectiveCamera(float FOV, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::PERSPECTIVE;

		m_PerspectiveFOV = FOV;
		m_PerspectiveNearClip = nearClip;
		m_PerspectiveFarClip = farClip;

		RecalculateProjectionMatrix();
	}

	void SceneCamera::SetViewportSize(uint width, uint height)
	{
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjectionMatrix();
	}

	void SceneCamera::RecalculateProjectionMatrix()
	{
		if (m_ProjectionType == ProjectionType::PERSPECTIVE)
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