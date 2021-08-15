#ifndef _RENDERCOMMAND_H_
#define _RENDERCOMMAND_H_

#include "RendererAPI.h"

namespace Kaimos {

	//A renderer is a high-level class, a full-on renderer (doesn't deals with commands such as ClearScene), it deals with high-level constructs (scenes, meshes...)
	//RenderCommands should NOT do multiple things, they are just commands (unless specifically suposed-to)
	class RenderCommand
	{
	public:

		// --- Public Class Methods ---
		inline static void Init()																		{ s_RendererAPI->Init(); }

		// --- Public RendererAPI Methods ---
		inline static void Clear()																		{ s_RendererAPI->Clear(); }
		inline static void SetClearColor(const glm::vec4& color)										{ s_RendererAPI->SetClearColor(color); }

		inline static void DrawIndexed(const Ref<VertexArray>& vertex_array, uint index_count = 0)		{ s_RendererAPI->DrawIndexed(vertex_array, index_count); }
		inline static void DrawUnindexed(const Ref<VertexArray>& vertex_array, uint count)				{ s_RendererAPI->DrawUnindexed(vertex_array, count); }
		inline static void SetViewport(uint x, uint y, uint width, uint height)							{ s_RendererAPI->SetViewport(x, y, width, height); }

	private:

		static ScopePtr<RendererAPI> s_RendererAPI;
	};

}

#endif //_RENDERCOMMAND_H_
