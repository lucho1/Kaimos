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

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint indexCount = 0) const override;

		virtual void SetViewport(uint x, uint y, uint width, uint height) override;
	};

}

#endif