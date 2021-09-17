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

	void CameraController::OnUpdate(Timestep dt, bool viewport_focused, const glm::vec3& focus_pos)
	{
		m_IsBlockingLBtn = m_IsMoving = false;
		const glm::vec2& mouse_pos = Input::GetMousePos();
		if (viewport_focused)
		{
			// -- Focus --
			if (Input::IsKeyPressed(KEY::F))
			{
				SetPosition(focus_pos - GetForwardVector() * m_ZoomLevel);
				SetOrientation(0.5f, 0.0f);
			}

			if (m_UsingGuizmo)
			{
				m_InitialMousePosition = mouse_pos;
				return;
			}

			// -- Mouse Movement Calc --
			glm::vec2 delta = (mouse_pos - m_InitialMousePosition) * 0.003f;

			// -- Mid Mouse Button --
			if (Input::IsMouseButtonPressed(MOUSE::BUTTON_MIDDLE))
				PanCamera(delta);

			// -- Left & Right Mouse Buttons --
			if (Input::IsMouseButtonPressed(MOUSE::BUTTON_LEFT) && Input::IsMouseButtonPressed(MOUSE::BUTTON_RIGHT))
			{
				m_IsMoving = true;
				if(Input::IsKeyPressed(KEY::LEFT_ALT))
					ZoomCamera(delta.y);
				else if(!m_UsingGuizmo)
					PanCamera(-delta);
			}
			else if (Input::IsMouseButtonPressed(MOUSE::BUTTON_LEFT))
			{
				m_IsMoving = true;
				m_IsBlockingLBtn = true;
				if (Input::IsKeyPressed(KEY::LEFT_ALT))
					OrbitCamera(delta);
				else
					AdvanceCamera(delta);
			}

			if (Input::IsMouseButtonPressed(MOUSE::BUTTON_RIGHT))
			{
				m_IsMoving = true;
				if (Input::IsKeyPressed(KEY::LEFT_ALT))
					ZoomCamera(delta.y);
				else
				{
					RotateCamera(delta);

					// -- FPS Movement --
					float speed = m_MoveSpeed * m_SpeedMultiplier;
					if (Input::IsKeyPressed(KEY::LEFT_SHIFT))
						speed *= 2.0f;

					if (Input::IsKeyPressed(KEY::W))
						SetPosition(m_Position + GetForwardVector() * (speed * dt * 12.0f));
					if (Input::IsKeyPressed(KEY::S))
						SetPosition(m_Position - GetForwardVector() * (speed * dt * 12.0f));
					if (Input::IsKeyPressed(KEY::A))
						SetPosition(m_Position - GetRightVector() * (speed * dt * 10.0f));
					if (Input::IsKeyPressed(KEY::D))
						SetPosition(m_Position + GetRightVector() * (speed * dt * 10.0f));
					if (Input::IsKeyPressed(KEY::Q))
						SetPosition(m_Position - glm::vec3(0.0f, 1.0f, 0.0f) * (speed * dt * 10.0f));
					if (Input::IsKeyPressed(KEY::E))
						SetPosition(m_Position + glm::vec3(0.0f, 1.0f, 0.0f) * (speed * dt * 10.0f));
				}
			}
		}

		m_InitialMousePosition = mouse_pos;
	}

	void CameraController::OnEvent(Event& ev)
	{
		KS_PROFILE_FUNCTION();
		EventDispatcher dispatcher(ev);
		dispatcher.Dispatch<MouseScrolledEvent>(KS_BIND_EVENT_FN(CameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(KS_BIND_EVENT_FN(CameraController::OnWindowResized));
	}



	// ----------------------- Camera Getters/Setters -----------------------------------------------------	
	void CameraController::SetOrientation(const glm::vec2& angles)
	{
		SetOrientation(angles.x, angles.y);
	}

	void CameraController::SetOrientation(float x_angle, float y_angle)
	{
		if (!m_LockRotation)
		{
			m_Pitch = x_angle;
			m_Yaw = y_angle;
			m_FocalPoint = m_Position + GetForwardVector() * m_ZoomLevel;
			RecalculateView();
		}
	}

	void CameraController::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		m_FocalPoint = m_Position + GetForwardVector() * m_ZoomLevel;
		RecalculateView();
	}



	// ----------------------- Private Event Methods ------------------------------------------------------
	bool CameraController::OnMouseScrolled(MouseScrolledEvent& ev)
	{
		KS_PROFILE_FUNCTION();

		if (!Input::IsMouseButtonPressed(MOUSE::BUTTON_RIGHT))
			ZoomCamera(ev.GetYOffset()*0.1f);

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
		// -- Rotation --
		if (!m_LockRotation)
		{
			float yaw = GetUpVector().y < 0.0f ? -1.0f : 1.0f;
			m_Yaw += yaw * movement.x * m_RotationSpeed * 1.5f;
		}		

		// -- Movement Calc. --
		glm::vec3 fw = -GetForwardVector();
		glm::vec3 up = -GetUpVector();
		glm::vec3 dir = glm::vec3(fw.x, 0.0f, fw.z);

		// Cap movement if fw vec is too slope
		if (glm::dot(-GetForwardVector(), { 0.0f, 1.0f, 0.0f }) > 0.99f)
			dir = glm::vec3(up.x, 0.0f, up.z);

		// -- Move --
		m_Position += dir * movement.y * m_MoveSpeed * m_ZoomLevel * 2.0f;
		m_FocalPoint = m_Position + GetForwardVector() * m_ZoomLevel;
		RecalculateView();
	}

	void CameraController::CalculateRotation(const glm::vec2 rotation)
	{
		float yaw = GetUpVector().y < 0.0f ? -1.0f : 1.0f;
		m_Yaw += yaw * rotation.x * m_RotationSpeed;
		m_Pitch += rotation.y * m_RotationSpeed;
		m_Pitch = std::clamp(m_Pitch, -1.56f, 1.56f);
	}

	void CameraController::RotateCamera(const glm::vec2& rotation)
	{
		if (!m_LockRotation)
		{
			CalculateRotation(rotation);
			m_FocalPoint = m_Position + GetForwardVector() * m_ZoomLevel;
			RecalculateView();
		}
	}

	void CameraController::OrbitCamera(const glm::vec2& rotation)
	{
		if (!m_LockRotation)
		{
			CalculateRotation(rotation);
			RecalculateView();
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
		RecalculateView();
	}

	void CameraController::ZoomCamera(float zoom)
	{
		// -- Zoom Calc --
		float d = std::max(m_ZoomLevel * 0.35f, 0.0f);
		m_ZoomLevel -= zoom * std::min(d * d, m_MaxZoomSpeed);
		m_ZoomLevel = std::clamp(m_ZoomLevel, 1.75f, 75.0f);

		// -- View Recalc --
		RecalculateView();
	}

	void CameraController::RecalculateView()
	{
		m_Position = m_FocalPoint - GetForwardVector() * m_ZoomLevel;
		m_Camera.CalculateViewMatrix(m_Position, GetOrientation());
	}
}
