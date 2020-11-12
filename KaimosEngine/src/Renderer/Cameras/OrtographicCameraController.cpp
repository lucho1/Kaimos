#include "kspch.h"
#include "OrthographicCamera.h"
#include "OrtographicCameraController.h"

#include "Core/Input/Input.h"
#include "Core//Input/KaimosInputCodes.h"

namespace Kaimos {

	OrtographicCameraController::OrtographicCameraController(float aspect_ratio, bool activate_rotation)
		: m_AspectRatio(aspect_ratio), m_RotationActive(activate_rotation), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel) // From -AR to +AR (X) and -Zoom to +Zoom (Y)
	{
	}

	void OrtographicCameraController::OnUpdate(Timestep dt)
	{
		// -- Camera Movement --
		m_CameraMoveSpeed = m_ZoomLevel * m_SpeedMultiplier;
		if (Input::IsKeyPressed(KS_KEY_D))
		{
			m_CameraPos.x += cos(glm::radians(m_CameraRotation)) * m_CameraMoveSpeed * dt;
			m_CameraPos.y += sin(glm::radians(m_CameraRotation)) * m_CameraMoveSpeed * dt;
		}
		
		if (Input::IsKeyPressed(KS_KEY_A))
		{
			m_CameraPos.x -= cos(glm::radians(m_CameraRotation)) * m_CameraMoveSpeed * dt;
			m_CameraPos.y -= sin(glm::radians(m_CameraRotation)) * m_CameraMoveSpeed * dt;
		}
		
		if (Input::IsKeyPressed(KS_KEY_W))
		{
			m_CameraPos.x += -sin(glm::radians(m_CameraRotation)) * m_CameraMoveSpeed * dt;
			m_CameraPos.y += cos(glm::radians(m_CameraRotation)) * m_CameraMoveSpeed * dt;
		}
		
		if (Input::IsKeyPressed(KS_KEY_S))
		{
			m_CameraPos.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraMoveSpeed * dt;
			m_CameraPos.y -= cos(glm::radians(m_CameraRotation)) * m_CameraMoveSpeed * dt;
		}

		if (Input::IsKeyPressed(KS_KEY_F))
			m_CameraPos = glm::vec3(0.0f);
		
		// -- Camera Rotation --
		if (m_RotationActive)
		{
			if (Input::IsKeyPressed(KS_KEY_E))
				m_CameraRotation -= m_CameraRotSpeed * dt;
			else if (Input::IsKeyPressed(KS_KEY_Q))
				m_CameraRotation += m_CameraRotSpeed * dt;

			if (m_CameraRotation > 180.0f)
				m_CameraRotation -= 360.0f;
			else if (m_CameraRotation <= -180.0f)
				m_CameraRotation += 360.0f;

			m_Camera.RecalculateViewMatrix(m_CameraPos, m_CameraRotation);
		}
		else
			m_Camera.RecalculateViewMatrix(m_CameraPos);
	}

	void OrtographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(KS_BIND_EVENT_FN(OrtographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(KS_BIND_EVENT_FN(OrtographicCameraController::OnWindowResized));
	}

	bool OrtographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

		//m_CameraMoveSpeed = m_ZoomLevel * m_SpeedMultiplier;
		return false;
	}

	bool OrtographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.GetWidth()/(float)e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

	// --- Setters ---
	void OrtographicCameraController::SetZoomLevel(float zoom_level)
	{
		m_ZoomLevel = zoom_level;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		
		//m_CameraMoveSpeed = m_ZoomLevel * m_SpeedMultiplier;
	}

	void OrtographicCameraController::SetAspectRatio(float aspect_ratio)
	{
		m_AspectRatio = aspect_ratio;
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	void OrtographicCameraController::SetAspectRatio(float width, float height)
	{
		SetAspectRatio(width/height);
	}
}