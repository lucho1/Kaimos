#include "kspch.h"
#include "Framebuffer.h"

#include "Renderer/Renderer.h"
#include "Renderer/OpenGL/Resources/OGLFrameBuffer.h"

namespace Kaimos {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSettings& settings, bool generate_depth_renderbuffer)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OGLFramebuffer>(settings, generate_depth_renderbuffer);
			case RendererAPI::API::NONE:		KS_FATAL_ERROR("RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_FATAL_ERROR("RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}

	Ref<Framebuffer> Framebuffer::CreateEmptyAndBind(uint width, uint height, bool generate_depth_renderbuffer)
	{
		switch (Renderer::GetRendererAPI())
		{
		case RendererAPI::API::OPENGL:		return CreateRef<OGLFramebuffer>(width, height, generate_depth_renderbuffer);
		case RendererAPI::API::NONE:		KS_FATAL_ERROR("RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_FATAL_ERROR("RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}
}
