#ifndef _RENDERERAPI_H_
#define _RENDERERAPI_H_

#include <glm/glm.hpp>
#include "Buffer.h"

namespace Kaimos {

	class RendererAPI
	{
	public:

		enum class API { NONE = 0, OPENGL = 1 };

	public:

		virtual void SetClearColor(const glm::vec4& color) const = 0;
		virtual void Clear() const = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) const = 0;

		inline static const API GetAPI() { return s_API; }

	private:

		static API s_API;
	};
}


#endif