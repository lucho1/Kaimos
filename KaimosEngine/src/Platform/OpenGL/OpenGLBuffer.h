#ifndef _OPENGLBUFFER_H_
#define _OPENGLBUFFER_H_

#include "Renderer/Buffer.h"

namespace Kaimos {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:

		OpenGLVertexBuffer(float* vertices, uint size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const;
		virtual void Unbind() const;

	private:

		uint m_BufferID = 0;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:

		OpenGLIndexBuffer(uint* vertices, uint count);
		virtual ~OpenGLIndexBuffer();

		virtual uint GetCount() const { return m_Count; }

		virtual void Bind() const;
		virtual void Unbind() const;

	private:

		uint m_BufferID = 0;
		uint m_Count = 0;
	};

}

#endif