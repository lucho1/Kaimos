#include "kspch.h"
#include "Texture.h"

#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Kaimos {

	// ----------------------- Public Texture2D Methods ---------------------------------------------------
	Ref<Texture2D> Texture2D::Create(uint width, uint height)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OpenGLTexture2D>(width, height);
			case RendererAPI::API::NONE:		KS_ENGINE_ASSERT(false, "RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_ENGINE_ASSERT(false, "RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OpenGLTexture2D>(path);
			case RendererAPI::API::NONE:		KS_ENGINE_ASSERT(false, "RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_ENGINE_ASSERT(false, "RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}
}
