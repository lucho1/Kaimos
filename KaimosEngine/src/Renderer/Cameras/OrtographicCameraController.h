#ifndef _ORTOGRAPHIC_CAMERA_CONTROLLER_H_
#define _ORTOGRAPHIC_CAMERA_CONTROLLER_H_

#include "OrthographicCamera.h"
#include "Core/Time/Timestep.h"

#include "Core/Events/ApplicationEvent.h"
#include "Core/Events/MouseEvent.h"

namespace Kaimos {

	struct OrthographicCameraBounds
	{
		float Left = 0.0f, Right = 1.0f, Bottom = 0.0f, Top = 1.0f;
		float GetWidth()	const { return Right - Left;}
		float GetHeight()	const { return Top - Bottom;}
	};

	class OrtographicCameraController
	{
	public:
		
		// --- Public Class/Event Methods ---
		OrtographicCameraController(float aspect_ratio, bool activate_rotation = false);

		void OnUpdate(Timestep dt);
		void OnEvent(Event& e);

	public:

		// --- Getters ---
		inline const OrthographicCamera& GetCamera()		const { return m_Camera; }
		inline const OrthographicCameraBounds& GetBounds()	const { return m_CamBounds; }

		inline float GetZoomLevel()							const { return m_ZoomLevel; }
		inline float GetAspectRatio()						const { return m_AspectRatio; }

		inline float GetRotationSpeed()						const { return m_CameraRotSpeed; }
		inline float GetMovementSpeed()						const { return m_SpeedMultiplier; }
		
		inline glm::vec3 GetPosition()						const { return m_CameraPos; }
		inline float GetRotation()							const { return m_CameraRotation; }
		inline bool IsRotationActive()						const { return m_RotationActive; }

		// --- Setters ---
		void SetZoomLevel(float zoom_level);
		void SetAspectRatio(float width, float height);
		void SetAspectRatio(float aspect_ratio);
		
		inline void SetRotationSpeed(float rot_speed)	{ m_CameraRotSpeed = rot_speed; }
		inline void SetMovementSpeed(float mov_speed)	{ m_SpeedMultiplier = mov_speed; }

		inline void SetPosition(const glm::vec3& pos)	{ m_CameraPos = pos; }
		inline void SetRotation(float rot)				{ m_CameraRotation = rot; }
		inline void ActivateRotation(bool activate)		{ m_RotationActive = activate; }


	private:

		// --- Private Event Methods ---
		// Scroll changes zoom level whereas resize changes Aspect Ratio
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

	private:

		float m_AspectRatio, m_ZoomLevel = 1.0f;
		glm::vec3 m_CameraPos = glm::vec3(0.0f);

		bool m_RotationActive = true;
		float m_CameraMoveSpeed = 1.0f, m_SpeedMultiplier = 1.0f;
		float m_CameraRotSpeed = 15.0f, m_CameraRotation = 0.0f; // Degrees, anti-clockwise - Also, OrthoCameras rotate only over 1 axis, so we only need a float
		
		OrthographicCameraBounds m_CamBounds;
		OrthographicCamera m_Camera;
	};
}

#endif //_ORTOGRAPHIC_CAMERA_CONTROLLER_H_
