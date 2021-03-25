#ifndef _EDITORCAMERA_H_
#define _EDITORCAMERA_H_

#include "Camera.h"
#include "Core/Time/Timestep.h"
#include "Core/Events/Event.h"
#include "Core/Events/MouseEvent.h"

#include <glm/glm.hpp>

namespace Kaimos
{
	class EditorCamera : public Camera
	{
	public:

		// --- Public Class Methods ---
		EditorCamera() = default;
		EditorCamera(float FOV, float aspect_ratio, float near_clip, float far_clip);

		// --- Public Class/Event Methods ---
		void OnUpdate(Timestep dt);
		void OnEvent(Event& ev);

	public:

		// --- Getters ---
		inline const glm::vec3& GetPosition()	const { return m_Position; }
		glm::quat GetOrientation()				const;

		inline float GetYaw()					const { return m_Yaw; }
		inline float GetPitch()					const { return m_Pitch; }
		inline float GetDistance()				const { return m_Distance; }


		const glm::mat4& GetViewMatrix()		const { return m_ViewMatrix; }
		glm::mat4 GetViewProjection()			const { return m_ProjectionMatrix * m_ViewMatrix; }

		glm::vec3 GetUpVector()					const;
		glm::vec3 GetRightVector()				const;
		glm::vec3 GetForwardVector()			const;

	public:
		
		// --- Setters ---
		inline void SetDistance(float distance)						{ m_Distance = distance; }
		inline void SetViewportSize(float width, float height)		{ m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjectionMatrix(); }

		inline void LockRotation(bool lock)							{ m_LockRotation = lock; }

	private:

		// --- Private Camera Methods ---
		void UpdateViewMatrix();
		void UpdateProjectionMatrix();
		glm::vec3 CalculatePosition();
		
		// --- Private Camera Settings Methods ---
		void MouseRotate(const glm::vec2& rot);
		void MousePan(const glm::vec2& pan);
		void MouseZoom(float zoom);

		float RotationSpeed()			const;
		glm::vec2 PanSpeed()			const;
		float ZoomSpeed()				const;

		// --- Private Event Methods ---
		bool OnMouseScroll(MouseScrolledEvent& ev);

	private:

		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 10000.0f;

		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::vec3 m_Position = glm::vec3(0.0f);
		glm::vec3 m_FocalPoint = glm::vec3(0.0f);

		glm::vec2 m_InitialMousePosition = glm::vec2(0.0f);

		float m_Distance = 10.0f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;
		bool m_LockRotation = false;

		float m_ViewportWidth = 1280.0f, m_ViewportHeight = 720.0f;
	};
}

#endif //_EDITORCAMERA_H_
