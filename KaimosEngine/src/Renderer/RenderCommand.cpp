#include "kspch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Kaimos {

	ScopePtr<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}