#ifndef _RENDERERAPI_H_
#define _RENDERERAPI_H_

#include <glm/glm.hpp>
#include "Renderer/Resources/Buffer.h"

namespace Kaimos {

	class RendererAPI
	{
	public:

		enum class API { NONE = 0, OPENGL = 1 };

	public:

		// --- Public Class Methods ---
		virtual ~RendererAPI() = default;
		virtual void Init() = 0;


		// --- Public RendererAPI Methods ---
		virtual void SetClearColor(const glm::vec4& color) const = 0;
		virtual void Clear() const = 0;
		
		virtual void DrawIndexed(const Ref<VertexArray>& vertex_array, uint index_count = 0) const = 0;
		virtual void SetViewport(uint x, uint y, uint width, uint height) = 0;


		static ScopePtr<RendererAPI> Create();
		
		// --- Getters ---
		inline static const API GetAPI() { return s_API; }

	private:

		static API s_API;
	};
}

#endif //_RENDERERAPI_H_
