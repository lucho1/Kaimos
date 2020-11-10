#include "kspch.h"
#include "Texture.h"

#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Kaimos {

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return std::make_shared<OpenGLTexture2D>(path);
			case RendererAPI::API::NONE:		KS_ENGINE_ASSERT(false, "RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_ENGINE_ASSERT(false, "RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}
}