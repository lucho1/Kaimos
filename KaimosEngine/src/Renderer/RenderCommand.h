#ifndef _RENDERCOMMAND_H_
#define _RENDERCOMMAND_H_

#include "RendererAPI.h"

namespace Kaimos {

	class RenderCommand
	{
	public:

		inline static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }
		inline static void Clear() { s_RendererAPI->Clear(); }

		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) { s_RendererAPI->DrawIndexed(vertexArray); }

	private:

		static RendererAPI* s_RendererAPI;
	};

}

#endif