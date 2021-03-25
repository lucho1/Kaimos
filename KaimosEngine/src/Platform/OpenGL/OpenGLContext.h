#ifndef _OPENGLCONTEXT_H_
#define _OPENGLCONTEXT_H_

#include "Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Kaimos {

	class OpenGLContext : public GraphicsContext
	{
	public:

		// --- Public Class Methods ---
		OpenGLContext(GLFWwindow* glfw_window_handle);
		virtual void Init() override;

		// --- Public RendererAPI Methods ---
		virtual void SwapBuffers() override;

	private:

		GLFWwindow* m_WindowHandle = nullptr;
	};
}

#endif //_OPENGLCONTEXT_H_
