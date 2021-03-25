#ifndef _OGLBUFFER_H_
#define _OGLBUFFER_H_

#include "Renderer/Buffer.h"

namespace Kaimos {

	// ---- VERTEX BUFFER ----
	class OGLVertexBuffer : public VertexBuffer
	{
	public:

		// --- Public Class Methods ---
		OGLVertexBuffer(float* vertices, uint size);
		OGLVertexBuffer(uint size);
		virtual ~OGLVertexBuffer();

		// --- Public Vertex Buffer Methods ---
		virtual void Bind() const override;
		virtual void Unbind() const override;

		// --- Getters/Setters ---
		virtual const BufferLayout& GetLayout()				const override	{ return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout)	override		{ m_Layout = layout; }

		virtual void SetData(const void* data, uint size)	override;

	private:

		uint m_BufferID = 0;
		BufferLayout m_Layout;
	};



	// ---- INDEX BUFFER ----
	class OGLIndexBuffer : public IndexBuffer
	{
	public:

		// --- Public Class Methods ---
		OGLIndexBuffer(uint* vertices, uint count);
		virtual ~OGLIndexBuffer();

		// --- Public Index Buffer Methods ---
		virtual void Bind() const override;
		virtual void Unbind() const override;
		
		// -- Getters --
		virtual uint GetCount() const { return m_Count; }

	private:

		uint m_BufferID = 0;
		uint m_Count = 0;
	};



	// ---- VERTEX ARRAY ----
	class OGLVertexArray : public VertexArray
	{
	public:

		// --- Public Class Methods ---
		OGLVertexArray();
		virtual ~OGLVertexArray();

		// --- Public Vertex Array Methods ---
		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertex_buffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& index_buffer) override;

		// --- Getters ---
		inline virtual const Ref<IndexBuffer>& GetIndexBuffer()					const override { return m_IndexBuffer; }
		inline virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers()	const override { return m_VertexBuffers; }

	private:

		Ref<IndexBuffer> m_IndexBuffer = nullptr;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;

		uint m_VArrayID = 0;
		uint m_VBufferIndex = 0;
	};
}

#endif //_OGLBUFFER_H_
