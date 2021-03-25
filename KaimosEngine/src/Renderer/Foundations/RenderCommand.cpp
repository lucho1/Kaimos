#include "kspch.h"
#include "RenderCommand.h"
#include "Renderer/OpenGL/OGLRendererAPI.h"

namespace Kaimos
{
	ScopePtr<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}
