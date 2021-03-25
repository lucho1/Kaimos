#ifndef _SCENE_CAMERA_H_
#define _SCENE_CAMERA_H_

#include "Renderer/Cameras/Camera.h"

namespace Kaimos {

	// TODO: Delete this on Camera Rework
	class SceneCamera : public Camera
	{
	public:

		enum class PROJECTION_TYPE { PERSPECTIVE = 0, ORTHOGRAPHIC = 1 };

	public:

		// --- Public Class Methods ---
		SceneCamera();
		virtual ~SceneCamera() = default;

		// --- Public Camera Methods ---
		void SetViewportSize(uint width, uint height);

	public:

		// --- Orthographic Camera G/Setters ---
		inline PROJECTION_TYPE GetProjectionType()	const { return m_ProjectionType; }
		inline float GetOrthographicSize()			const { return m_OrthoSize; }
		inline float GetOrthographicNearClip()		const { return m_OrthographicNearClip; }
		inline float GetOrthographicFarClip()		const { return m_OrthographicFarClip; }
		
		void SetProjectionType(PROJECTION_TYPE projection)					{ m_ProjectionType = projection; RecalculateProjectionMatrix(); }
		void SetOrthographicSize(float size)								{ m_OrthoSize = size; RecalculateProjectionMatrix(); }
		void SetOrthographicClips(float near_plane, float far_plane)		{ m_OrthographicNearClip = near_plane; m_OrthographicFarClip = far_plane; RecalculateProjectionMatrix(); }
		
		void SetOrthographicCamera(float size, float near_clip, float far_clip);
		
	public:

		// --- Perspective Camera G/Setters ---
		inline float GetPerspectiveFOV()			const	{ return m_PerspectiveFOV; }
		inline float GetPerspectiveNearClip()		const	{ return m_PerspectiveNearClip; }
		inline float GetPerspectiveFarClip()		const	{ return m_PerspectiveFarClip; }
		
		void SetPerspectiveCamera(float FOV, float near_clip, float far_clip);
		
		void SetPerspectiveFOV(float FOV)							{ m_PerspectiveFOV = FOV; RecalculateProjectionMatrix(); }
		void SetPerspectiveClips(float near_plane, float farClip)	{ m_PerspectiveNearClip = near_plane; m_PerspectiveFarClip = farClip; RecalculateProjectionMatrix(); }

	private:

		// --- Private Camera Methods ---
		void RecalculateProjectionMatrix();

	private:

		PROJECTION_TYPE m_ProjectionType = PROJECTION_TYPE::ORTHOGRAPHIC;

		float m_OrthoSize = 10.0f;
		float m_OrthographicNearClip = -1.0f, m_OrthographicFarClip = 1.0f;

		float m_PerspectiveFOV = 45.0f;
		float m_PerspectiveNearClip = 0.01f, m_PerspectiveFarClip = 10000.0f;

		float m_AspectRatio = 1.7f;
	};
}

#endif //_SCENE_CAMERA_H_
