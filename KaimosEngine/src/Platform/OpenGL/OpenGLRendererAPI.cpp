#include "kspch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Kaimos {

	void OpenGLRendererAPI::Init()
	{
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

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray) const
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0); // TODO/OJU: Should we actually do this?
	}

	void OpenGLRendererAPI::SetViewport(uint x, uint y, uint width, uint height)
	{
		glViewport(x, y, width, height);
	}
}