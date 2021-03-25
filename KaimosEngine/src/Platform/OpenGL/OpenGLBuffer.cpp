#include "kspch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Kaimos {

	// ----------------------- Globals --------------------------------------------------------------------
	// Cast own data type to OpenGL data type
	static GLenum ShaderDataTypeToOpenGLType(SHADER_DATATYPE type)
	{
		switch (type)
		{
			case SHADER_DATATYPE::FLOAT:		return GL_FLOAT;
			case SHADER_DATATYPE::FLOAT2:		return GL_FLOAT;
			case SHADER_DATATYPE::FLOAT3:		return GL_FLOAT;
			case SHADER_DATATYPE::FLOAT4:		return GL_FLOAT;
			case SHADER_DATATYPE::MAT3:			return GL_FLOAT;
			case SHADER_DATATYPE::MAT4:			return GL_FLOAT;
			case SHADER_DATATYPE::INT:			return GL_INT;
			case SHADER_DATATYPE::INT2:			return GL_INT;
			case SHADER_DATATYPE::INT3:			return GL_INT;
			case SHADER_DATATYPE::INT4:			return GL_INT;
			case SHADER_DATATYPE::BOOL:			return GL_BOOL;
		}

		KS_ENGINE_ASSERT(false, "ShaderDataType passed Unknown or Incorrect!");
		return (GLenum)0;
	}

	

	// ---------------------------- VERTEX BUFFER ---------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint size)
	{
		KS_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint size)
	{
		KS_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		KS_PROFILE_FUNCTION();
		glDeleteBuffers(1, &m_BufferID);
	}

	

	// ----------------------- Public Vertex Buffer Methods -----------------------------------------------
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

	

	// ----------------------- Getters/Setters ------------------------------------------------------------
	void OpenGLVertexBuffer::SetData(const void* data, uint size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}



	// ---------------------------- INDEX BUFFER ----------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint* vertices, uint count)
		: m_Count(count)
	{
		KS_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_BufferID);
		
		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint), vertices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		KS_PROFILE_FUNCTION();
		glDeleteBuffers(1, &m_BufferID);
	}

	

	// ----------------------- Public Index Buffer Methods ------------------------------------------------
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



	// ---------------------------- VERTEX ARRAY ----------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
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


	
	// ----------------------- Public Vertex Array Methods ------------------------------------------------
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


	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertex_buffer)
	{
		KS_PROFILE_FUNCTION();
		KS_ENGINE_ASSERT(vertex_buffer->GetLayout().GetElements().size(), "VertexBuffer has not layouts!");
		glBindVertexArray(m_VArrayID);
		vertex_buffer->Bind();

		const auto& layout = vertex_buffer->GetLayout();
		for (const auto& element : layout)
		{
			switch (element.Type)
			{
				case SHADER_DATATYPE::FLOAT:
				case SHADER_DATATYPE::FLOAT2:
				case SHADER_DATATYPE::FLOAT3:
				case SHADER_DATATYPE::FLOAT4:
				{
					glEnableVertexAttribArray(m_VBufferIndex);
					glVertexAttribPointer(m_VBufferIndex, element.GetElementTypeCount(),
						ShaderDataTypeToOpenGLType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(), (const void*)element.Offset);

					++m_VBufferIndex;
					break;
				}
				case SHADER_DATATYPE::INT:
				case SHADER_DATATYPE::INT2:
				case SHADER_DATATYPE::INT3:
				case SHADER_DATATYPE::INT4:
				case SHADER_DATATYPE::BOOL:
				{
					glEnableVertexAttribArray(m_VBufferIndex);
					glVertexAttribIPointer(m_VBufferIndex, element.GetElementTypeCount(),
						ShaderDataTypeToOpenGLType(element.Type),
						layout.GetStride(), (const void*)element.Offset);

					++m_VBufferIndex;
					break;
				}
				case SHADER_DATATYPE::MAT3:
				case SHADER_DATATYPE::MAT4:
				{
					uint8_t count = element.GetElementTypeCount();
					for (uint8_t i = 0; i < count; ++i)
					{
						glEnableVertexAttribArray(m_VBufferIndex);
						glVertexAttribPointer(m_VBufferIndex, count,
							ShaderDataTypeToOpenGLType(element.Type),
							element.Normalized ? GL_TRUE : GL_FALSE,
							layout.GetStride(), (const void*)(element.Offset + sizeof(float) * count * i));

						glVertexAttribDivisor(m_VBufferIndex, 1);
						++m_VBufferIndex;
					}

					break;
				}
			}
		}

		m_VertexBuffers.push_back(vertex_buffer);
	}


	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& index_buffer)
	{
		KS_PROFILE_FUNCTION();
		glBindVertexArray(m_VArrayID);
		index_buffer->Bind();

		m_IndexBuffer = index_buffer;
	}
}
