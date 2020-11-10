#ifndef _OPENGLRENDERERAPI_
#define _OPENGLRENDERERAPI_

#include "Renderer/RendererAPI.h"

namespace Kaimos {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:

		virtual void Init() override;
		virtual void SetClearColor(const glm::vec4& color) const override;
		virtual void Clear() const override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) const override;
	};

}

#endif