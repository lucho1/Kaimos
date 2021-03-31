#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/glm.hpp>

namespace Kaimos {

	enum class CAMERA_PROJECTION { PERSPECTIVE = 0, ORTHOGRAPHIC };

	class Camera
	{
		friend class CameraController;
	public:

		// --- Public Class Methods ---
		Camera();
		Camera(float FOV, float ar = 1.778f, float nclip = 0.01f, float fclip = 10000.0f);
		virtual ~Camera() = default;

		// --- Public Camera Methods ---
		// Sets the camera's Viewport
		void SetOrthographicParameters(float ortho_size = 10.0f, float nclip = -1.0f, float fclip = 1.0f);
		void SetPerspectiveParameters(float FOV = 45.0f, float nclip = 0.1f, float fclip = 10000.0f);

	public:

		// --- Getters ---
		// Only for cameras with Orthographic Perspective
		inline float GetSize()							const { return m_OrthographicSize; }
		inline float GetNearClip()						const { return m_NearClip; }
		inline float GetFarClip()						const { return m_FarClip; }
		inline float GetFOV()							const { return m_FOV; }
		inline float GetAspectRato()					const { return m_AR; }
		inline glm::ivec2 GetViewportSize()				const { return m_ViewportSize; }

		
		inline glm::mat4 GetViewProjection()			const { return m_Projection * m_View; }		// PxV since OGL is column-major, though in DX should be VxP (careful! TODO!)
		inline const glm::mat4& GetView()				const { return m_View; }
		inline const glm::mat4& GetProjection()			const { return m_Projection; }
		inline CAMERA_PROJECTION GetProjectionType()	const { return m_ProjectionType; }

	public:

		// --- Setters ---
		void SetViewport(uint width, uint height);
		inline void SetFOV(float FOV)							{ m_FOV = FOV;			CalculateProjectionMatrix(); }
		inline void SetNearClip(float nclip)					{ m_NearClip = nclip;	CalculateProjectionMatrix(); }
		inline void SetFarClip(float fclip)						{ m_FarClip = fclip;	CalculateProjectionMatrix(); }

		// Only for cameras with Orthographic Perspective
		inline void SetSize(float size)							{ m_OrthographicSize = size; CalculateProjectionMatrix(); }

	protected:

		// --- Private Camera Methods ---
		void CalculateViewMatrix(const glm::vec3& position, const glm::quat& orientation);
		void CalculateProjectionMatrix();

	private:

		CAMERA_PROJECTION m_ProjectionType = CAMERA_PROJECTION::PERSPECTIVE;
		glm::mat4 m_Projection = glm::mat4(1.0f);
		glm::mat4 m_View = glm::mat4(1.0f);

		float m_AR = 1.778f, m_FOV = 45.0f, m_NearClip = 0.1f, m_FarClip = 10000.0f;
		float m_OrthographicSize = 10.0f;

		glm::ivec2 m_ViewportSize = glm::ivec2(1280, 720);
	};
}

#endif //_CAMERA_H_
