#include "kspch.h"
#include "Camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Kaimos {
	

	// ----------------------- Public Class Methods -------------------------------------------------------
	Camera::Camera()
	{
		CalculateProjectionMatrix();
		CalculateViewMatrix(glm::vec3(0.0f, 0.0f, 2.0f), glm::identity<glm::quat>());
	}

	Camera::Camera(float FOV, float ar, float nclip, float fclip) : m_FOV(FOV), m_AR(ar), m_NearClip(nclip), m_FarClip(fclip)
	{
		CalculateProjectionMatrix();
		CalculateViewMatrix(glm::vec3(0.0f, 0.0f, 2.0f), glm::identity<glm::quat>());
	}

	// ----------------------- Public Camera Methods ------------------------------------------------------
	void Kaimos::Camera::SetViewport(uint width, uint height)
	{
		if (height == 0)
		{
			KS_ENGINE_WARN("Setting Camera Viewport to 0!");
			return;
		}

		m_ViewportSize = glm::ivec2(width, height);
		m_AR = (float)width/(float)height;

		CalculateProjectionMatrix();
	}

	void Camera::SetOrthographicParameters(float ortho_size, float nclip, float fclip)
	{
		m_ProjectionType = CAMERA_PROJECTION::ORTHOGRAPHIC;
		m_OrthographicSize = ortho_size;
		m_NearClip = nclip;
		m_FarClip = fclip;
		CalculateProjectionMatrix();
	}

	void Camera::SetPerspectiveParameters(float FOV, float nclip, float fclip)
	{
		m_ProjectionType = CAMERA_PROJECTION::PERSPECTIVE;
		m_FOV = FOV;
		m_NearClip = nclip;
		m_FarClip = fclip;
		CalculateProjectionMatrix();
	}



	// ----------------------- Private Camera Methods -----------------------------------------------------
	void Camera::CalculateViewMatrix(const glm::vec3& position, const glm::quat& orientation)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(orientation);
		m_View = glm::inverse(transform);
	}

	void Kaimos::Camera::CalculateProjectionMatrix()
	{
		if (m_ProjectionType == CAMERA_PROJECTION::PERSPECTIVE)
			m_Projection = glm::perspective(glm::radians(m_FOV), m_AR, m_NearClip, m_FarClip);
		else
		{
			float horizontal = m_OrthographicSize * m_AR * 0.5f;
			float vertical = m_OrthographicSize * 0.5f;
			m_Projection = glm::ortho(-horizontal, horizontal, -vertical, vertical, m_NearClip, m_FarClip);
		}
	}
}
