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
		if (Input::IsKeyPressed(KS_KEY_D))
			m_CameraPos.x += m_CameraMoveSpeed * dt;
		
		if (Input::IsKeyPressed(KS_KEY_A))
			m_CameraPos.x -= m_CameraMoveSpeed * dt;
		
		if (Input::IsKeyPressed(KS_KEY_W))
			m_CameraPos.y += m_CameraMoveSpeed * dt;
		
		if (Input::IsKeyPressed(KS_KEY_S))
			m_CameraPos.y -= m_CameraMoveSpeed * dt;

		m_Camera.SetPosition(m_CameraPos);
		m_CameraMoveSpeed = m_ZoomLevel;
		
		// -- Camera Rotation --
		if (m_RotationActive)
		{
			if (Input::IsKeyPressed(KS_KEY_E))
				m_CameraRotation -= m_CameraRotSpeed * dt;
			else if (Input::IsKeyPressed(KS_KEY_Q))
				m_CameraRotation += m_CameraRotSpeed * dt;

			m_Camera.SetRotation(m_CameraRotation);
		}
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
		return false;
	}

	bool OrtographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.GetWidth()/(float)e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
}