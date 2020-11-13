#include "kspch.h"
#include "GraphicsContext.h"

#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Kaimos {

	ScopePtr<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::NONE: KS_ENGINE_ASSERT(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
			case RendererAPI::API::OPENGL: return CreateScopePtr<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		KS_ENGINE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}