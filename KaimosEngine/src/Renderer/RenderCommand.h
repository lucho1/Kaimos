#ifndef _RENDERCOMMAND_H_
#define _RENDERCOMMAND_H_

#include "RendererAPI.h"

namespace Kaimos {

	class RenderCommand
	{
	public:
		
		inline static void Init()									{ s_RendererAPI->Init(); }
		inline static void Clear()									{ s_RendererAPI->Clear(); }
		inline static void SetClearColor(const glm::vec4& color)	{ s_RendererAPI->SetClearColor(color); }

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)		{ s_RendererAPI->DrawIndexed(vertexArray); }

		inline static void SetViewport(uint x, uint y, uint width, uint height)	{ s_RendererAPI->SetViewport(x, y, width, height); }

	private:

		static ScopePtr<RendererAPI> s_RendererAPI;
	};

}

#endif