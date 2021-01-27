#ifndef _SCENE_CAMERA_H_
#define _SCENE_CAMERA_H_

#include "Renderer/Cameras/Camera.h"

namespace Kaimos {

	class SceneCamera : public Camera
	{
	public:

		enum class ProjectionType { PERSPECTIVE = 0, ORTHOGRAPHIC = 1 };

	public:

		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetViewportSize(uint width, uint height);

		void SetProjectionType(ProjectionType projection)	{ m_ProjectionType = projection; RecalculateProjectionMatrix(); }
		const ProjectionType GetProjectionType()	const	{ return m_ProjectionType; }

		// Orthographic Camera
		void SetOrhographicCamera(float size, float nearClip, float farClip);

		void SetOrthographicSize(float size)					{ m_OrthoSize = size; RecalculateProjectionMatrix(); }
		void SetOrthographicClips(float nearP, float farP)		{ m_OrthographicNearClip = nearP; m_OrthographicFarClip = farP; RecalculateProjectionMatrix(); }
		
		const float GetOrthographicSize()			const	{ return m_OrthoSize; }
		const float GetOrthographicNearClip()		const	{ return m_OrthographicNearClip; }
		const float GetOrthographicFarClip()		const	{ return m_OrthographicFarClip; }
		

		// Perspective Camera
		void SetPerspectiveCamera(float FOV, float nearClip, float farClip);

		void SetPerspectiveFOV(float FOV)					{ m_PerspectiveFOV = FOV; RecalculateProjectionMatrix(); }
		void SetPerspectiveClips(float nearP, float farP)	{ m_PerspectiveNearClip = nearP; m_PerspectiveFarClip = farP; RecalculateProjectionMatrix(); }

		const float GetPerspectiveFOV()				const	{ return m_PerspectiveFOV; }
		const float GetPerspectiveNearClip()		const	{ return m_PerspectiveNearClip; }
		const float GetPerspectiveFarClip()			const	{ return m_PerspectiveFarClip; }

	private:

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