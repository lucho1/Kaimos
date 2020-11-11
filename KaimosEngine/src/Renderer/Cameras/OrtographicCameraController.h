#ifndef _ORTOGRAPHIC_CAMERA_CONTROLLER_H_
#define _ORTOGRAPHIC_CAMERA_CONTROLLER_H_

#include "OrthographicCamera.h"
#include "Core/Time/Timestep.h"

#include "Core/Events/ApplicationEvent.h"
#include "Core/Events/MouseEvent.h"

namespace Kaimos {

	class OrtographicCameraController
	{
	public:
		
		OrtographicCameraController(float aspect_ratio, bool activate_rotation = false);

		void OnUpdate(Timestep dt);
		void OnEvent(Event& e);

		// --- Getters ---
		const OrthographicCamera& GetCamera() const { return m_Camera; }
		float GetZoomLevel() const { return m_ZoomLevel; }

		// --- Setters ---
		void SetZoomLevel(float zoom_level) { m_ZoomLevel = zoom_level; }

	private:

		// Scroll changes zoom level whereas resize changes Aspect Ratio
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

	private:

		float m_AspectRatio, m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;

		bool m_RotationActive;
		float m_CameraRotation = 0.0f, m_CameraMoveSpeed = 1.0f, m_CameraRotSpeed = 15.0f;
		glm::vec3 m_CameraPos = glm::vec3(0.0f);
	};
}

#endif