#include "kspch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Kaimos {

	// --- Function to cast data type to OpenGL data type ---
	static GLenum ShaderDataTypeToOpenGLType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:			return GL_FLOAT;
			case ShaderDataType::Float2:		return GL_FLOAT;
			case ShaderDataType::Float3:		return GL_FLOAT;
			case ShaderDataType::Float4:		return GL_FLOAT;
			case ShaderDataType::Mat3:			return GL_FLOAT;
			case ShaderDataType::Mat4:			return GL_FLOAT;
			case ShaderDataType::Int:			return GL_INT;
			case ShaderDataType::Int2:			return GL_INT;
			case ShaderDataType::Int3:			return GL_INT;
			case ShaderDataType::Int4:			return GL_INT;
			case ShaderDataType::Bool:			return GL_BOOL;
		}

		KS_ENGINE_ASSERT(false, "ShaderDataType passed Unknown or Incorrect!");
		return (GLenum)0;
	}


	// --- VERTEX BUFFER ---
	// ------------------------------------------------------------------------
	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint size)
	{
		KS_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		KS_PROFILE_FUNCTION();
		glDeleteBuffers(1, &m_BufferID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		KS_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		KS_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// --- INDEX BUFFER ---
	// ------------------------------------------------------------------------
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint* vertices, uint count)
		: m_Count(count)
	{
		KS_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID); //Also, GL_ELEMENT_ARRAY_BUFFER
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint), vertices, GL_STATIC_DRAW); //Also, GL_ELEMENT_ARRAY_BUFFER
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		KS_PROFILE_FUNCTION();
		glDeleteBuffers(1, &m_BufferID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		KS_PROFILE_FUNCTION();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		KS_PROFILE_FUNCTION();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// --- VERTEX ARRAY ---
	// ------------------------------------------------------------------------
	OpenGLVertexArray::OpenGLVertexArray()
	{
		KS_PROFILE_FUNCTION();
		glCreateVertexArrays(1, &m_VArrayID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		KS_PROFILE_FUNCTION();
		glDeleteVertexArrays(1, &m_VArrayID);
	}

	void OpenGLVertexArray::Bind() const
	{
		KS_PROFILE_FUNCTION();
		glBindVertexArray(m_VArrayID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		KS_PROFILE_FUNCTION();
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& Vbuffer)
	{
		KS_PROFILE_FUNCTION();
		KS_ENGINE_ASSERT(Vbuffer->GetLayout().GetElements().size(), "VertexBuffer has not layouts!");
		glBindVertexArray(m_VArrayID);
		Vbuffer->Bind();

		const auto& layout = Vbuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(m_VBufferIndex);
			glVertexAttribPointer(	m_VBufferIndex, element.GetElementTypeCount(),
									ShaderDataTypeToOpenGLType(element.Type),
									element.Normalized ? GL_TRUE : GL_FALSE,
									layout.GetStride(), (const void*)element.Offset);
			++m_VBufferIndex;
		}

		m_VertexBuffers.push_back(Vbuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& Ibuffer)
	{
		KS_PROFILE_FUNCTION();
		glBindVertexArray(m_VArrayID);
		Ibuffer->Bind();

		m_IndexBuffer = Ibuffer;
	}

}