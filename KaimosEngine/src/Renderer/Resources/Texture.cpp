#include "kspch.h"
#include "Texture.h"

#include "Renderer/Renderer.h"
#include "Renderer/OpenGL/Resources/OGLTexture.h"

namespace Kaimos {

	Ref<Texture2D> Texture2D::Create(uint width, uint height)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OGLTexture2D>(width, height);
			case RendererAPI::API::NONE:		KS_FATAL_ERROR("RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_FATAL_ERROR("RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& filepath)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OGLTexture2D>(filepath);
			case RendererAPI::API::NONE:		KS_FATAL_ERROR("RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_FATAL_ERROR("RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}



	Ref<HDRTexture2D> HDRTexture2D::Create(const std::string& filepath)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OGL_HDRTexture2D>(filepath);
			case RendererAPI::API::NONE:		KS_FATAL_ERROR("RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_FATAL_ERROR("RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}


	
	Ref<LUTTexture> LUTTexture::Create(uint size)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OGL_LUTTexture>(size);
			case RendererAPI::API::NONE:		KS_FATAL_ERROR("RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_FATAL_ERROR("RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}



	Ref<CubemapTexture> CubemapTexture::Create(uint width, uint height, bool linear_mipmap_filtering)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OGL_CubemapTexture>(width, height, linear_mipmap_filtering);
			case RendererAPI::API::NONE:		KS_FATAL_ERROR("RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_FATAL_ERROR("RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}
}
