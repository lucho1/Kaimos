#ifndef _OPENGLRENDERERAPI_
#define _OPENGLRENDERERAPI_

#include "Renderer/RendererAPI.h"

namespace Kaimos {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:

		virtual void SetClearColor(const glm::vec4& color) const override;
		virtual void Clear() const override;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) const override;
	};

}

#endif