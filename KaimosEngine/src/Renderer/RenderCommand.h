#ifndef _RENDERCOMMAND_H_
#define _RENDERCOMMAND_H_

#include "RendererAPI.h"

namespace Kaimos {

	//A renderer is a high-level class, a full-on renderer (doesn't deals with commands such as ClearScene), it deals with high-level constructs (scenes, meshes...)
	//RenderCommands should NOT do multiple things, they are just commands (unless specifically suposed-to)
	class RenderCommand
	{
	public:
		
		inline static void Init()																		{ s_RendererAPI->Init(); }
		inline static void Clear()																		{ s_RendererAPI->Clear(); }
		inline static void SetClearColor(const glm::vec4& color)										{ s_RendererAPI->SetClearColor(color); }

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint indexCount = 0)		{ s_RendererAPI->DrawIndexed(vertexArray, indexCount); }

		inline static void SetViewport(uint x, uint y, uint width, uint height)							{ s_RendererAPI->SetViewport(x, y, width, height); }

	private:

		static ScopePtr<RendererAPI> s_RendererAPI;
	};

}

#endif