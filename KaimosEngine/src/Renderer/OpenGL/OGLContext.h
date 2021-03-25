#ifndef _OPENGLCONTEXT_H_
#define _OPENGLCONTEXT_H_

#include "Renderer/Foundations/GraphicsContext.h"

struct GLFWwindow;

namespace Kaimos {

	class OGLContext : public GraphicsContext
	{
	public:

		// --- Public Class Methods ---
		OGLContext(GLFWwindow* glfw_window_handle);
		virtual void Init() override;

		// --- Public RendererAPI Methods ---
		virtual void SwapBuffers() override;

	private:

		GLFWwindow* m_WindowHandle = nullptr;
	};
}

#endif //_OPENGLCONTEXT_H_
