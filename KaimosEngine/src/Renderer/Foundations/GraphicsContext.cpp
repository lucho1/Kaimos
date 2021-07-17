#include "kspch.h"
#include "GraphicsContext.h"

#include "Renderer/Renderer.h"
#include "Renderer/OpenGL/OGLContext.h"

namespace Kaimos {

	ScopePtr<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::NONE:	KS_FATAL_ERROR("RendererAPI::NONE is currently not supported!"); return nullptr;
			case RendererAPI::API::OPENGL:	return CreateScopePtr<OGLContext>(static_cast<GLFWwindow*>(window));
		}

		KS_FATAL_ERROR("Unknown RendererAPI!");
		return nullptr;
	}
}
