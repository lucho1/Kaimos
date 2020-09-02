#include "kspch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Kaimos {


	// Here we decide which rendering API we are using, thus which kind of class type we instantiate/return
	VertexBuffer* VertexBuffer::Create(float* vertices, uint size)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RENDERER_API::OPENGL:		return new OpenGLVertexBuffer(vertices, size);
			case RENDERER_API::NONE:		KS_ENGINE_ASSERT(false, "RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_ENGINE_ASSERT(false, "RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}

	// Here we decide which rendering API we are using, thus which kind of class type we instantiate/return
	IndexBuffer* IndexBuffer::Create(uint* vertices, uint count)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RENDERER_API::OPENGL:		return new OpenGLIndexBuffer(vertices, count);
			case RENDERER_API::NONE:		KS_ENGINE_ASSERT(false, "RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_ENGINE_ASSERT(false, "RendererAPI is unknown, not selected or failed!");
		return nullptr;
	}

}