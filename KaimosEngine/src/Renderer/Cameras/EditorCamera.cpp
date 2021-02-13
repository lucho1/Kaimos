#include "kspch.h"
#include "EditorCamera.h"

#include "Core/Input/Input.h"
#include "Core/Input/KaimosInputCodes.h"

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


namespace Kaimos {

	// --- CLASS METHODS ---
	Kaimos::EditorCamera::EditorCamera(float FOV, float aspect_ratio, float nClip, float fClip)
		: m_FOV(FOV), m_AspectRatio(aspect_ratio), m_NearClip(nClip), m_FarClip(fClip), Camera(glm::perspective(glm::radians(FOV), aspect_ratio, nClip, fClip))
	{
		UpdateViewMatrix();
	}

	void Kaimos::EditorCamera::OnUpdate(Timestep dt)
	{
		if (Input::IsKeyPressed(KEY::LEFT_ALT))
		{
			const glm::vec2& mouse_pos = Input::GetMousePos();
			glm::vec2 delta = (mouse_pos - m_InitialMousePosition) * 0.003f;
			m_InitialMousePosition = mouse_pos;

			if (Input::IsMouseButtonPressed(MOUSE::BUTTON_MIDDLE))
				MousePan(delta);
			if (Input::IsMouseButtonPressed(MOUSE::BUTTON_LEFT))
				MouseRotate(delta);
			if (Input::IsMouseButtonPressed(MOUSE::BUTTON_RIGHT))
				MouseZoom(delta.y);
		}

		UpdateViewMatrix();
	}

	void Kaimos::EditorCamera::OnEvent(Event& ev)
	{
		EventDispatcher dispatcher(ev);
		dispatcher.Dispatch<MouseScrolledEvent>(KS_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}


	// --- CAMERA METHODS ---
	glm::vec3 Kaimos::EditorCamera::GetUpVector() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 Kaimos::EditorCamera::GetRightVector() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0, 0.0f));
	}

	glm::vec3 Kaimos::EditorCamera::GetForwardVector() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::quat Kaimos::EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

	glm::vec3 Kaimos::EditorCamera::CalculatePosition()
	{
		return m_FocalPoint - GetForwardVector() * m_Distance;
	}

	void Kaimos::EditorCamera::UpdateProjectionMatrix()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void Kaimos::EditorCamera::UpdateViewMatrix()
	{
		//Lock Cam Rotation
		if (m_LockRotation)
			m_Yaw = m_Pitch = 0.0;

		// Get position & Orientation
		m_Position = CalculatePosition();
		glm::quat orientation = GetOrientation();

		// Update Matrix
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}


	// --- EVENTS METHODS ---
	bool Kaimos::EditorCamera::OnMouseScroll(MouseScrolledEvent& ev)
	{
		float zoom = ev.GetYOffset() * 0.1f;
		MouseZoom(zoom);
		UpdateViewMatrix();
		return false;
	}

	void Kaimos::EditorCamera::MousePan(const glm::vec2& pan)
	{
		glm::vec2 speed = PanSpeed();
		m_FocalPoint += -GetRightVector() * pan.x * speed.x * m_Distance;
		m_FocalPoint += GetUpVector() * pan.y * speed.y * m_Distance;
	}

	void Kaimos::EditorCamera::MouseRotate(const glm::vec2& rot)
	{
		float yaw = GetUpVector().y < 0.0f ? -1.0f : 1.0f;
		m_Yaw += yaw * rot.x * RotationSpeed();
		m_Pitch += rot.y * RotationSpeed();
	}

	void Kaimos::EditorCamera::MouseZoom(float zoom)
	{
		m_Distance -= zoom * ZoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetForwardVector();
			m_Distance = 1.0f;
		}
	}

	glm::vec2 Kaimos::EditorCamera::PanSpeed() const
	{
		// This works with a quadratic function for dx and dy: x squared - x * constant
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // Max PanSpeed 2.4
		float dx = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f);
		float dy = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { dx, dy };
	}

	float Kaimos::EditorCamera::RotationSpeed() const
	{
		return 0.8f;
	}

	float Kaimos::EditorCamera::ZoomSpeed() const
	{
		float d = std::max(m_Distance * 0.2f, 0.0f);
		float speed = std::min(d * d, 100.0f); // Max Speed is 100
		return speed;
	}
}
