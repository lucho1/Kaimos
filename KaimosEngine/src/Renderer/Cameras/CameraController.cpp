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
				CameraPanning(delta);
			if (Input::IsMouseButtonPressed(MOUSE::BUTTON_LEFT))
				CameraRotation(delta);
			//if (Input::IsMouseButtonPressed(MOUSE::BUTTON_RIGHT))
			//	MouseZoom(delta.y);
		}

		//UpdateViewMatrix()
		//if(m_LockRotation) // TODO: If locked, rotation !changed
		m_Position = m_FocalPoint - GetForwardVector() * m_ZoomLevel; //CalculatePosition();
		m_Camera.CalculateViewMatrix(m_Position, GetOrientation());
	}

	void CameraController::OnEvent(Event& ev)
	{
		KS_PROFILE_FUNCTION();
		EventDispatcher dispatcher(ev);
		dispatcher.Dispatch<MouseScrolledEvent>(KS_BIND_EVENT_FN(CameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(KS_BIND_EVENT_FN(CameraController::OnWindowResized));
	}


	
	// ----------------------- Private Event Methods ------------------------------------------------------
	bool CameraController::OnMouseScrolled(MouseScrolledEvent& ev)
	{
		KS_PROFILE_FUNCTION();
		
		float zoom = ev.GetYOffset() * 0.1f;

		// MouseZoom()
		float d = std::max(m_ZoomLevel * 0.2f, 0.0f);
		float speed = std::min(d * d, 100.0f); //TODO: Max Zoom Speed (100.0f)
		m_ZoomLevel -= zoom * speed;

		if (m_ZoomLevel < 1.0f) // TODO: Min Zoom Level (1.0f)
		{
			m_FocalPoint += GetForwardVector();
			m_ZoomLevel = 1.0f;
		}

		// UpdateMatrix()
		//if(m_LockRotation) // TODO: If locked, rotation !changed
		m_Position = m_FocalPoint - GetForwardVector() * m_ZoomLevel; //CalculatePosition();
		m_Camera.CalculateViewMatrix(m_Position, GetOrientation());

		return false;
	}

	bool CameraController::OnWindowResized(WindowResizeEvent& ev)
	{
		KS_PROFILE_FUNCTION();
		m_Camera.SetAspectRato((float)ev.GetWidth() / (float)ev.GetHeight());
		return false;
	}

	

	// ----------------------- Private Camera Methods -----------------------------------------------------
	void CameraController::CameraRotation(const glm::vec2& rotation)
	{
		float yaw = GetUpVector().y < 0.0f ? -1.0f : 1.0f;

		m_Yaw += yaw * rotation.x * m_RotationSpeed;
		m_Pitch += rotation.y * m_RotationSpeed;
	}

	void CameraController::CameraPanning(const glm::vec2& panning)
	{
		// PanSpeed()
		// This works with a quadratic function for dx and dy: x squared - x * constant
		float x = std::min(1280.0f / 1000.0f, 2.4f); // TODO: Max PanSpeed (2.4f) & some way to get the viewport width & height (1280.0f & 720.0f)
		float dx = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(720.0f / 1000.0f, 2.4f);
		float dy = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;


		// Pan()
		glm::vec2 speed = { dx, dy };
		m_FocalPoint += -GetRightVector() * panning.x * speed.x * m_ZoomLevel;
		m_FocalPoint += GetUpVector() * panning.y * speed.y * m_ZoomLevel;
	}
}
