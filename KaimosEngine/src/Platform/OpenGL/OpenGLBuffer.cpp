#include "kspch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Kaimos {

	// --- VERTEX BUFFER ---
	// ------------------------------------------------------------------------
	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint size)
	{
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// --- INDEX BUFFER ---
	// ------------------------------------------------------------------------
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint* vertices, uint count)
		: m_Count(count)
	{
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), vertices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}