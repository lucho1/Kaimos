#ifndef _SCENE_CAMERA_H_
#define _SCENE_CAMERA_H_

#include "Renderer/Cameras/Camera.h"

namespace Kaimos {

	class SceneCamera : public Camera
	{
	public:

		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrhographicCamera(float size, float nearClip, float farClip);
		void SetViewportSize(uint width, uint height);

		void SetOrthographicSize(float size)			{ m_OrthoSize = size; RecalculateProjectionMatrix(); }
		const float GetOrthographicSize()		const	{ return m_OrthoSize; }

	private:

		void RecalculateProjectionMatrix();

	private:

		float m_OrthoSize = 10.0f;
		float m_NearClip = -1.0f, m_FarClip = 1.0f;
		float m_AspectRatio = 1.7f;
	};
}
#endif //_SCENE_CAMERA_H_