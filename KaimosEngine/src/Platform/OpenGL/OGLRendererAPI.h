#ifndef _OPENGLRENDERERAPI_
#define _OPENGLRENDERERAPI_

#include "Renderer/RendererAPI.h"

namespace Kaimos {

	class OGLRendererAPI : public RendererAPI
	{
	public:

		// --- Public Class Methods ---
		virtual void Init() override;

		// --- Public RendererAPI Methods ---
		virtual void SetClearColor(const glm::vec4& color) const override;
		virtual void Clear() const override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertex_array, uint index_count = 0) const override;
		virtual void SetViewport(uint x, uint y, uint width, uint height) override;
	};
}

#endif //_OPENGLRENDERERAPI_
