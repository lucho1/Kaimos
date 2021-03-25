#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/glm.hpp>

namespace Kaimos {

	class Camera
	{
	public:

		// --- Public Class Methods ---
		Camera() = default;
		Camera(const glm::mat4& projection) : m_ProjectionMatrix(projection) {}

		virtual ~Camera() = default;

		// --- Getters ---
		inline const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }

	protected:
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
	};

}

#endif //_CAMERA_H_
