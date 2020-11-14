#include "kspch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Kaimos {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left, right, bottom, top)), m_ViewMatrix(1.0f)
	{
		KS_PROFILE_FUNCTION();
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		KS_PROFILE_FUNCTION();

		m_ProjectionMatrix = glm::ortho(left, right, bottom, top);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix(glm::vec3 pos, float rotation)
	{
		KS_PROFILE_FUNCTION();

		glm::mat4 transform =	glm::translate(glm::mat4(1.0f), pos) *
								glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1)) *  // As stated, orthographic cameras only rotate in 1 axis
								glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

		m_ViewMatrix = glm::inverse(transform); // Inverse since camera looks inverse of its movement/orientation
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix; // PxV instead of VxP (as it should) since OGL is column-major, so it has to be this order (DX isn't, so we should do VxP)
	}
}