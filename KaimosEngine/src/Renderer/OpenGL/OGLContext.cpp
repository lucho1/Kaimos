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
		KS_ENGINE_TRACE("Initializing Glad");

		// -- Communicating to GLFW which is the Current Context --
		glfwMakeContextCurrent(m_WindowHandle);

		// -- Glad Initialization --
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		KS_ENGINE_ASSERT(status, "Couldn't Initialize Glad!");

		int v_maj, v_min;
		glGetIntegerv(GL_MAJOR_VERSION, &v_maj);
		glGetIntegerv(GL_MINOR_VERSION, &v_min);
		KS_ENGINE_ASSERT((v_maj == 4 && v_min <= 6), "Wrong OpenGL version!");
	}



	// ----------------------- Public RendererAPI Methods -------------------------------------------------
	void OGLContext::SwapBuffers()
	{
		KS_PROFILE_FUNCTION();
		glfwSwapBuffers(m_WindowHandle);
	}
}
