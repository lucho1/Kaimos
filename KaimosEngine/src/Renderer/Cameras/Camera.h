#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/glm.hpp>

namespace Kaimos {

	class Camera
	{
	public:

		Camera(const glm::mat4& projection) : m_Projection(projection) {}
		const const glm::mat4& GetProjection() const { return m_Projection; }

	private:
		glm::mat4 m_Projection;
	};

}

#endif //_CAMERA_H_