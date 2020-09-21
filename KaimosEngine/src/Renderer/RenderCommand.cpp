#include "kspch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Kaimos {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}