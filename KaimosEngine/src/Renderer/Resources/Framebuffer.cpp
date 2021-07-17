#include "kspch.h"
#include "Framebuffer.h"

#include "Renderer/Renderer.h"
#include "Renderer/OpenGL/Resources/OGLFrameBuffer.h"

namespace Kaimos {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSettings& settings)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OGLFramebuffer>(settings);
			case RendererAPI::API::NONE:		KS_FATAL_ERROR("RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_FATAL_ERROR("RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}
}
