#ifndef _SCENE_CAMERA_H_
#define _SCENE_CAMERA_H_

#include "Renderer/Cameras/Camera.h"

namespace Kaimos {

	class SceneCamera : public Camera
	{
	public:

		enum class ProjectionType { PERSPECTIVE = 0, ORTHOGRAPHIC = 1 };

	public:

		// --- Public Class Methods ---
		SceneCamera();
		virtual ~SceneCamera() = default;

		// --- Public Camera Methods ---
		void SetViewportSize(uint width, uint height);

	public:

		// --- Orthographic Camera G/Setters ---
		ProjectionType GetProjectionType()	const { return m_ProjectionType; }
		float GetOrthographicSize()			const { return m_OrthoSize; }
		float GetOrthographicNearClip()		const { return m_OrthographicNearClip; }
		float GetOrthographicFarClip()		const { return m_OrthographicFarClip; }
		
		void SetProjectionType(ProjectionType projection)					{ m_ProjectionType = projection; RecalculateProjectionMatrix(); }
		void SetOrthographicSize(float size)								{ m_OrthoSize = size; RecalculateProjectionMatrix(); }
		void SetOrthographicClips(float near_plane, float far_plane)		{ m_OrthographicNearClip = near_plane; m_OrthographicFarClip = far_plane; RecalculateProjectionMatrix(); }
		
		void SetOrthographicCamera(float size, float near_clip, float far_clip);
		
	public:

		// --- Perspective Camera G/Setters ---
		float GetPerspectiveFOV()			const	{ return m_PerspectiveFOV; }
		float GetPerspectiveNearClip()		const	{ return m_PerspectiveNearClip; }
		float GetPerspectiveFarClip()		const	{ return m_PerspectiveFarClip; }
		
		void SetPerspectiveCamera(float FOV, float near_clip, float far_clip);
		
		void SetPerspectiveFOV(float FOV)							{ m_PerspectiveFOV = FOV; RecalculateProjectionMatrix(); }
		void SetPerspectiveClips(float near_plane, float farClip)	{ m_PerspectiveNearClip = near_plane; m_PerspectiveFarClip = farClip; RecalculateProjectionMatrix(); }

	private:

		// --- Private Camera Methods ---
		void RecalculateProjectionMatrix();

	private:

		ProjectionType m_ProjectionType = ProjectionType::ORTHOGRAPHIC;

		float m_OrthoSize = 10.0f;
		float m_OrthographicNearClip = -1.0f, m_OrthographicFarClip = 1.0f;

		float m_PerspectiveFOV = 45.0f;
		float m_PerspectiveNearClip = 0.01f, m_PerspectiveFarClip = 10000.0f;

		float m_AspectRatio = 1.7f;
	};
}

#endif //_SCENE_CAMERA_H_
