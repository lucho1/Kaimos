#include "kspch.h"
#include "Buffer.h"
#include "Renderer/Renderer.h"

#include "Renderer/OpenGL/Resources/OGLBuffer.h"

namespace Kaimos {


	// ----------------------- Vertex Buffer Creation -----------------------------------------------------
	// Here we decide which rendering API we are using, thus which kind of class type we instantiate/return
	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint size)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OGLVertexBuffer>(vertices, size);
			case RendererAPI::API::NONE:		KS_FATAL_ERROR("RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_FATAL_ERROR("RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint size)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OGLVertexBuffer>(size);
			case RendererAPI::API::NONE:		KS_FATAL_ERROR("RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_FATAL_ERROR("RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}

	

	// ----------------------- Index Buffer Creation ------------------------------------------------------
	Ref<IndexBuffer> IndexBuffer::Create(uint* vertices, uint count)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OGLIndexBuffer>(vertices, count);
			case RendererAPI::API::NONE:		KS_FATAL_ERROR("RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_FATAL_ERROR("RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint count)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OGLIndexBuffer>(count);
			case RendererAPI::API::NONE:		KS_FATAL_ERROR("RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_FATAL_ERROR("RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}


	
	// ----------------------- Vertex Array Creation ------------------------------------------------------
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OGLVertexArray>();
			case RendererAPI::API::NONE:		KS_FATAL_ERROR("RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_FATAL_ERROR("RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}
}
