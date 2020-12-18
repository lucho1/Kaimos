#include "kspch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Kaimos {


	// Here we decide which rendering API we are using, thus which kind of class type we instantiate/return
	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint size)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OpenGLVertexBuffer>(vertices, size);
			case RendererAPI::API::NONE:		KS_ENGINE_ASSERT(false, "RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_ENGINE_ASSERT(false, "RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint size)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OpenGLVertexBuffer>(size);
			case RendererAPI::API::NONE:		KS_ENGINE_ASSERT(false, "RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_ENGINE_ASSERT(false, "RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}

	// Here we decide which rendering API we are using, thus which kind of class type we instantiate/return
	Ref<IndexBuffer> IndexBuffer::Create(uint* vertices, uint count)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OpenGLIndexBuffer>(vertices, count);
			case RendererAPI::API::NONE:		KS_ENGINE_ASSERT(false, "RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_ENGINE_ASSERT(false, "RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}

	// Here we decide which rendering API we are using, thus which kind of class type we instantiate/return
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OpenGLVertexArray>();
			case RendererAPI::API::NONE:		KS_ENGINE_ASSERT(false, "RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_ENGINE_ASSERT(false, "RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}

}