#ifndef _CAMERACONTROLLER_H_
#define _CAMERACONTROLLER_H_

#include "Camera.h"
#include "Core/Utils/Time/Timestep.h"

#include "Core/Application/Events/ApplicationEvent.h"
#include "Core/Application/Events/MouseEvent.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Kaimos {

	class CameraController
	{
		friend class ToolbarPanel;
	public:

		// --- Public Class/Event Methods ---
		CameraController();
		CameraController(Camera& camera);
		~CameraController() = default;

		void OnUpdate(Timestep dt, bool viewport_focused);
		void OnEvent(Event& ev);

	public:

		// --- Camera Transform Getters ---
		inline glm::vec3 GetPosition()		const { return m_Position; }
		inline glm::quat GetOrientation()	const { return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f)); }

		glm::vec3 GetUpVector()				const { return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f)); }
		glm::vec3 GetRightVector()			const { return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f)); }
		glm::vec3 GetForwardVector()		const { return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f,-1.0f)); }
		
		// --- Camera Getters/Setters ---
		inline const Camera& GetCamera()	const { return m_Camera; }
		inline void SetCameraViewport(uint width, uint height) { m_Camera.SetViewport(width, height); }
		
	public:

		// --- Camera Transform Setters ---
		inline void SetOrientation(float x_angle, float y_angle);
		inline void SetPosition(const glm::vec3& position);

		// --- Camera Parameters Setters ---
		inline void SetZoomLevel(float zoom_level)					{ m_ZoomLevel = zoom_level; }
		inline void SetMoveSpeed(float speed)						{ m_MoveSpeed = speed; }
		inline void SetRotationSpeed(float speed)					{ m_RotationSpeed = speed; }
		inline void SetSpeedMultiplier(float multiplier)			{ m_SpeedMultiplier = multiplier; }
		inline void SetMaxZoomSpeed(float speed)					{ m_MaxZoomSpeed = speed; }

		inline void LockRotation(bool lock)							{ m_LockRotation = lock; }
		inline bool IsRotationLocked()								const { return m_LockRotation; }

	private:

		// --- Private Event Methods ---
		// Scroll changes Zoom
		bool OnMouseScrolled(MouseScrolledEvent& ev);
		// Resize changes Aspect Ratio
		bool OnWindowResized(WindowResizeEvent& ev);

		// --- Private Camera Methods ---
		void AdvanceCamera(const glm::vec2& movement);
		void RotateCamera(const glm::vec2& movement);
		void OrbitCamera(const glm::vec2& rotation);
		void PanCamera(const glm::vec2& panning);
		void ZoomCamera(float zoom);

		void RecalculateView();

	private:

		// --- Camera ---
		Camera m_Camera;

		// --- Camera Transform Parameters ---
		glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 2.0f);
		float m_Pitch = 0.0f, m_Yaw = 0.0f;

		glm::vec3 m_FocalPoint = glm::vec3(0.0f);
		glm::vec2 m_InitialMousePosition = glm::vec2(0.0f);

		// --- Camera Parameters ---
		float m_ZoomLevel = 10.0f, m_MoveSpeed = 1.0f, m_RotationSpeed = 0.8f, m_PanSpeed = 2.4f;
		float m_SpeedMultiplier = 1.0f, m_MaxZoomSpeed = 100.0f;

		bool m_LockRotation = false;
	};
}

#endif //_CAMERACONTROLLER_H_
