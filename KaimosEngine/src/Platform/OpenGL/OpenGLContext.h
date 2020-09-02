#ifndef _OPENGLCONTEXT_H_
#define _OPENGLCONTEXT_H_

#include "Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Kaimos {

	class OpenGLContext : public GraphicsContext
	{
	public:

		OpenGLContext(GLFWwindow* glfw_windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:

		GLFWwindow* m_WindowHandle;
	};

}

#endif