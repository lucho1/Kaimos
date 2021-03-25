#include "kspch.h"
#include "Framebuffer.h"

#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OGLFrameBuffer.h"

namespace Kaimos {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSettings& settings)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OGLFramebuffer>(settings);
			case RendererAPI::API::NONE:		KS_ENGINE_ASSERT(false, "RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_ENGINE_ASSERT(false, "RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}
}
