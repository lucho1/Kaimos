#include "kspch.h"
#include "CameraController.h"

#include "Core/Application/Input/Input.h"
#include "Core/Application/Input/KaimosInputCodes.h"


namespace Kaimos {

	
	// ----------------------- Public Class/Event Methods -------------------------------------------------
	CameraController::CameraController()
	{
		RecalculateView();
		m_Camera.CalculateProjectionMatrix();
	}

	CameraController::CameraController(Camera& camera) : m_Camera(camera)
	{
		RecalculateView();
		m_Camera.CalculateProjectionMatrix();
	}

	void CameraController::OnUpdate(Timestep dt, bool viewport_focused)
	{
		//bool recalculate_matrix = false;
		const glm::vec2& mouse_pos = Input::GetMousePos();

		if (viewport_focused)
		{
			glm::vec2 delta = (mouse_pos - m_InitialMousePosition) * 0.003f;
			if (Input::IsMouseButtonPressed(MOUSE::BUTTON_MIDDLE))
				PanCamera(delta);

			if (Input::IsMouseButtonPressed(MOUSE::BUTTON_LEFT) && Input::IsMouseButtonPressed(MOUSE::BUTTON_RIGHT))
			{
				if(Input::IsKeyPressed(KEY::LEFT_ALT))
					ZoomCamera(delta.y);
				else
					PanCamera(-delta);
			}
			else if (Input::IsMouseButtonPressed(MOUSE::BUTTON_LEFT))
			{
				if (Input::IsKeyPressed(KEY::LEFT_ALT))
					OrbitCamera(delta);
				else
					AdvanceCamera(delta);
			}
			else if (Input::IsMouseButtonPressed(MOUSE::BUTTON_RIGHT))
			{
				if (Input::IsKeyPressed(KEY::LEFT_ALT))
					ZoomCamera(delta.y);
				else
					RotateCamera(delta);
			}

		}


		m_InitialMousePosition = mouse_pos;
		//if(recalculate_matrix)
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
	inline void CameraController::SetOrientation(float x_angle, float y_angle)
	{
		if (!m_LockRotation)
		{
			m_Pitch = x_angle;
			m_Yaw = y_angle;
			RecalculateView();
		}
	}

	inline void CameraController::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		m_FocalPoint = m_Position + GetForwardVector() * m_ZoomLevel;
		RecalculateView();
	}



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
	void CameraController::AdvanceCamera(const glm::vec2& movement)
	{
		if (!m_LockRotation)
		{
			float yaw = GetUpVector().y < 0.0f ? -1.0f : 1.0f;
			m_Yaw += yaw * movement.x * m_RotationSpeed;
		}
		
		m_Position += -GetForwardVector() * movement.y;
		m_FocalPoint = m_Position + GetForwardVector() * m_ZoomLevel;
		RecalculateView();
	}

	void CameraController::RotateCamera(const glm::vec2& movement)
	{
		OrbitCamera(movement);
		if(!m_LockRotation)
			m_FocalPoint = m_Position + GetForwardVector() * m_ZoomLevel;
	}

	void CameraController::OrbitCamera(const glm::vec2& rotation)
	{
		if (!m_LockRotation)
		{
			float yaw = GetUpVector().y < 0.0f ? -1.0f : 1.0f;
			m_Yaw += yaw * rotation.x * m_RotationSpeed;
			m_Pitch += rotation.y * m_RotationSpeed;
		}
	}

	void CameraController::PanCamera(const glm::vec2& panning)
	{
		// -- Pan Speed on X and Y --
		glm::ivec2 viewport = m_Camera.GetViewportSize();
		float x = std::min((float)viewport.x / 1000.0f, m_PanSpeed);
		float y = std::min((float)viewport.y / 1000.0f, m_PanSpeed);

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
