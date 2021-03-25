#include "kspch.h"
#include "RendererAPI.h"

#include "Renderer/OpenGL/OGLRendererAPI.h"


namespace Kaimos {

	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OPENGL;

	ScopePtr<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
			case RendererAPI::API::NONE:	KS_ENGINE_ASSERT(false, "Renderer::API::NONE is currently NOT supported!"); return nullptr;
			case RendererAPI::API::OPENGL:	return CreateScopePtr<OGLRendererAPI>();
		}

		KS_ENGINE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
