#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "RenderCommand.h"

namespace Kaimos {

	class Renderer
	{
	public:

		// -- Methods --
		static void BeginScene();
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);

	public:

		// -- Getters --
		inline static const RendererAPI::API GetRendererAPI() { return RendererAPI::GetAPI(); }
	};
}

#endif