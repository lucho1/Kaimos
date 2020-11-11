#ifndef _ORTHOGRAPHICCAMERA_H_
#define _ORTHOGRAPHICCAMERA_H_

#include <glm/glm.hpp>

namespace Kaimos {

	class OrthographicCamera
	{
	public:

		OrthographicCamera(float left, float right, float bottom, float top);

		void SetProjection(float left, float right, float bottom, float top);
		
		inline void SetPosition(const glm::vec3& pos) { m_Position = pos; RecalculateViewMatrix(); }
		inline void SetRotation(const float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }
		

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

	private:

		void RecalculateViewMatrix();

	private:

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = glm::vec3(0.0f);
		float m_Rotation = 0.0f; // Orthographic Cameras rotate only over 1 axis, so we only need a float
	};

}

#endif