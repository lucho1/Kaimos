#include "kspch.h"
#include "RendererAPI.h"

#include "Renderer/OpenGL/OGLRendererAPI.h"


namespace Kaimos {

	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OPENGL;

	ScopePtr<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
			case RendererAPI::API::NONE:	KS_FATAL_ERROR("Renderer::API::NONE is currently NOT supported!"); return nullptr;
			case RendererAPI::API::OPENGL:	return CreateScopePtr<OGLRendererAPI>();
		}

		KS_FATAL_ERROR("Unknown RendererAPI!");
		return nullptr;
	}
}
