#ifndef _OPENGLBUFFER_H_
#define _OPENGLBUFFER_H_

#include "Renderer/Buffer.h"

namespace Kaimos {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:

		OpenGLVertexBuffer(float* vertices, uint size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

	private:

		uint m_BufferID = 0;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:

		OpenGLIndexBuffer(uint* vertices, uint count);
		virtual ~OpenGLIndexBuffer();

		virtual uint GetCount() const { return m_Count; }

		virtual void Bind() const override;
		virtual void Unbind() const override;

	private:

		uint m_BufferID = 0;
		uint m_Count = 0;
	};

	class OpenGLVertexArray : public VertexArray
	{
	public:

		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& Vbuffer) override;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& Ibuffer) override;
		virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

	private:

		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		uint m_ArrayID;
	};

}

#endif