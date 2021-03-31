#include "kspch.h"
#include "CameraController.h"

#include "Core/Application/Input/Input.h"
#include "Core/Application/Input/KaimosInputCodes.h"


namespace Kaimos {

	
	// ----------------------- Public Class/Event Methods -------------------------------------------------
	CameraController::CameraController()
	{
		m_Camera.CalculateProjectionMatrix();
		m_Camera.CalculateViewMatrix(m_Position, GetOrientation());
	}

	void CameraController::OnUpdate(Timestep dt)
	{
		if (Input::IsKeyPressed(KEY::LEFT_ALT))
		{
			const glm::vec2& mouse_pos = Input::GetMousePos();
			glm::vec2 delta = (mouse_pos - m_InitialMousePosition) * 0.003f;
			m_InitialMousePosition = mouse_pos;

			if (Input::IsMouseButtonPressed(MOUSE::BUTTON_MIDDLE))
				PanCamera(delta);
			if (Input::IsMouseButtonPressed(MOUSE::BUTTON_LEFT) && !m_LockRotation)
				RotateCamera(delta);
			if (Input::IsMouseButtonPressed(MOUSE::BUTTON_RIGHT))
				ZoomCamera(delta.y);
		}

		RecalculateView();
	}

	void CameraController::OnEvent(Event& ev)
	{
		KS_PROFILE_FUNCTION();
		EventDispatcher dispatcher(ev);
		dispatcher.Dispatch<MouseScrolledEvent>(KS_BIND_EVENT_FN(CameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(KS_BIND_EVENT_FN(CameraController::OnWindowResized));
	}



	// ----------------------- Camera Getters/Setters -----------------------------------------------------
	//void CameraController::SetOrientation(float x_angle, float y_angle)
	//{
	//	if (!m_LockRotation)
	//	{
	//		m_Pitch = x_angle;
	//		m_Yaw = y_angle;
	//		RecalculateView();
	//	}
	//}


	
	// ----------------------- Private Event Methods ------------------------------------------------------
	bool CameraController::OnMouseScrolled(MouseScrolledEvent& ev)
	{
		KS_PROFILE_FUNCTION();
		ZoomCamera(ev.GetYOffset() * 0.1f);
		return false;
	}

	bool CameraController::OnWindowResized(WindowResizeEvent& ev)
	{
		KS_PROFILE_FUNCTION();
		m_Camera.SetViewport(ev.GetWidth(), ev.GetHeight());
		return false;
	}

	

	// ----------------------- Private Camera Methods -----------------------------------------------------
	void CameraController::RotateCamera(const glm::vec2& rotation)
	{
		float yaw = GetUpVector().y < 0.0f ? -1.0f : 1.0f;
		m_Yaw += yaw * rotation.x * m_RotationSpeed;
		m_Pitch += rotation.y * m_RotationSpeed;
	}

	void CameraController::PanCamera(const glm::vec2& panning)
	{
		// -- Pan Speed on X and Y --
		glm::ivec2 viewport = m_Camera.GetViewportSize();
		float x = std::min((float)viewport.x / 1000.0f, m_MaxPanSpeed);
		float y = std::min((float)viewport.y / 1000.0f, m_MaxPanSpeed);

		// This is a quadratic function: X squared - X * constant
		float dx = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;
		float dy = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		// -- Pan --
		m_FocalPoint += -GetRightVector() * panning.x * dx * m_ZoomLevel;
		m_FocalPoint += GetUpVector() * panning.y * dy * m_ZoomLevel;
	}

	void CameraController::ZoomCamera(float zoom)
	{
		// -- Zoom Speed --
		float d = std::max(m_ZoomLevel * 0.2f, 0.0f);
		m_ZoomLevel -= zoom * std::min(d * d, m_MaxZoomSpeed);

		// -- Zoom Cap --
		if (m_ZoomLevel < 1.2f)
		{
			m_FocalPoint += GetForwardVector();
			m_ZoomLevel = 1.2f;
		}

		// -- View Recalc --
		RecalculateView();
	}

	void CameraController::RecalculateView()
	{
		m_Position = m_FocalPoint - GetForwardVector() * m_ZoomLevel;
		m_Camera.CalculateViewMatrix(m_Position, GetOrientation());
	}
}
