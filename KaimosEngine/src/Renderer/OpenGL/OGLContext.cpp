#include "kspch.h"
#include "OGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Kaimos {

	// ----------------------- Public Class Methods -------------------------------------------------------
	OGLContext::OGLContext(GLFWwindow* glfw_window_handle)
		: m_WindowHandle(glfw_window_handle)
	{
		KS_ENGINE_ASSERT(glfw_window_handle, "GLFW Window Handle is null or incorrect!");
	}

	void OGLContext::Init()
	{
		KS_PROFILE_FUNCTION();

		// -- Communicating to GLFW which is the Current Context --
		glfwMakeContextCurrent(m_WindowHandle);

		// -- Glad Initialization --
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		KS_ENGINE_ASSERT(status, "Couldn't Initialize Glad!");
		KS_ENGINE_TRACE("OpenGL Renderer Info: {0} {1} {2}", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

		int vMaj, vMin;
		glGetIntegerv(GL_MAJOR_VERSION, &vMaj);
		glGetIntegerv(GL_MINOR_VERSION, &vMin);
		//KS_ENGINE_ASSERT(vMaj > 4 || (vMaj == 4 && vMin >= 5), "OpenGL version should be over 4.5!"); // TODO: Fix asserts pls and review this
	}



	// ----------------------- Public RendererAPI Methods -------------------------------------------------
	void OGLContext::SwapBuffers()
	{
		KS_PROFILE_FUNCTION();
		glfwSwapBuffers(m_WindowHandle);
	}
}