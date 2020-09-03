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
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
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
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

	// --- VERTEX ARRAY ---
	// ------------------------------------------------------------------------
	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_ArrayID);
		//glBindVertexArray(0);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_ArrayID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_ArrayID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& Vbuffer)
	{
		KS_ENGINE_ASSERT(Vbuffer->GetLayout().GetElements().size(), "VertexBuffer has not layouts!");
		glBindVertexArray(m_ArrayID);
		Vbuffer->Bind();

		uint index = 0;
		const auto& layout = Vbuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(	index, element.GetElementTypeCount(),
									ShaderDataTypeToOpenGLType(element.Type),
									element.Normalized ? GL_TRUE : GL_FALSE,
									layout.GetStride(), (const void*)element.Offset);
			++index;
		}

		m_VertexBuffers.push_back(Vbuffer);

		//Vbuffer->Unbind();
		//glBindVertexArray(0);
	}

	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& Ibuffer)
	{
		glBindVertexArray(m_ArrayID);
		Ibuffer->Bind();

		m_IndexBuffer = Ibuffer;

		//Ibuffer->Unbind();
		//glBindVertexArray(0);
	}

}