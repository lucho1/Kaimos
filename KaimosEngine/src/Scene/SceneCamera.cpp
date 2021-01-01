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
		m_OrthoSize = size;
		m_NearClip = nearClip;
		m_FarClip = farClip;

		RecalculateProjectionMatrix();
	}

	void SceneCamera::SetViewportSize(uint width, uint height)
	{
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjectionMatrix();		
	}

	void SceneCamera::RecalculateProjectionMatrix()
	{
		float right = m_OrthoSize * m_AspectRatio * 0.5f;
		float top = m_OrthoSize * 0.5f;
		float left = -right;
		float bot = -top;

		m_ProjectionMatrix = glm::ortho(left, right, bot, top);
	}
}