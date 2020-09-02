#ifndef _RENDERER_H_
#define _RENDERER_H_

namespace Kaimos {

	enum class RENDERER_API { NONE = 0, OPENGL = 1 };

	class Renderer
	{
	public:
		inline static const RENDERER_API GetRendererAPI() { return s_RendererAPI; }
	private:
		static RENDERER_API s_RendererAPI;
	};
}

#endif