#ifndef _GRAPHICSCONTEXT_H_
#define _GRAPHICSCONTEXT_H_

namespace Kaimos {

	class GraphicsContext
	{
	public:

		// --- Public Class Methods ---
		virtual ~GraphicsContext() = default;
		virtual void Init() = 0;

		// --- Public RendererAPI Methods ---
		virtual void SwapBuffers() = 0;

		static ScopePtr<GraphicsContext> Create(void* window);
	};
}

#endif //_GRAPHICSCONTEXT_H_
