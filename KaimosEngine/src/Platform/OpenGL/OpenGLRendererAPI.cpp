#include "kspch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Kaimos {

	static void APIENTRY OpenGLMessageCallback(GLenum msg_source, GLenum msg_type, GLuint msg_id, GLenum msg_severity, GLsizei msg_length, const GLchar* msg, const void* userParam)
	{
		// Take into account that "ShaderRecompilation" message (#131218) warns you that there is a shader already bound on a shader-bind call
		std::string error_message = " --- OpenGL Error (#" + std::to_string(msg_id) +"): ";
		switch (msg_type)
		{
			case GL_DEBUG_TYPE_ERROR:				error_message += "GL Error";			break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	error_message += "Deprecated Behavior";	break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	error_message += "Undefined Behavior";	break;
			case GL_DEBUG_TYPE_PORTABILITY:			error_message += "Portability Error";	break;
			case GL_DEBUG_TYPE_PERFORMANCE:			error_message += "Performance Error";	break;
			case GL_DEBUG_TYPE_OTHER:				error_message += "Other Error";			break;
			default:								error_message += "Unknown Error";		break;
		}

		switch (msg_severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:         KS_ENGINE_CRITICAL((error_message + " - High Sev. -" + msg).c_str());	return;
			case GL_DEBUG_SEVERITY_MEDIUM:       KS_ENGINE_ERROR((error_message + " - Mid Sev. -" + msg).c_str());		return;
			case GL_DEBUG_SEVERITY_LOW:          KS_ENGINE_WARN((error_message + " - Low Sev. -" + msg).c_str());		return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: KS_ENGINE_TRACE((error_message + " - Notification -" + msg).c_str());	return;
		}

		KS_ENGINE_CRITICAL((error_message + " - UNKNOWN SEVERITY -" + msg).c_str());
		KS_ENGINE_ASSERT(false, "Error of Unknown Severity Level!");
	}

	void OpenGLRendererAPI::Init()
	{
		KS_PROFILE_FUNCTION();

		#ifdef KS_DEBUG
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLMessageCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
		#endif



		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color) const
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear() const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint indexCount) const
	{
		uint count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0); // TODO/OJU: Should we actually do this?
	}

	void OpenGLRendererAPI::SetViewport(uint x, uint y, uint width, uint height)
	{
		glViewport(x, y, width, height);
	}
}