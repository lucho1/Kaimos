#ifndef _GRAPHICSCONTEXT_H_
#define _GRAPHICSCONTEXT_H_

namespace Kaimos {

	class GraphicsContext
	{
	public:

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		static ScopePtr<GraphicsContext> Create(void* window);
	};
}

#endif