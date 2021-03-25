#include "kspch.h"
#include "OGLBuffer.h"

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
	OGLVertexBuffer::OGLVertexBuffer(float* vertices, uint size)
	{
		KS_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OGLVertexBuffer::OGLVertexBuffer(uint size)
	{
		KS_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OGLVertexBuffer::~OGLVertexBuffer()
	{
		KS_PROFILE_FUNCTION();
		glDeleteBuffers(1, &m_BufferID);
	}

	

	// ----------------------- Public Vertex Buffer Methods -----------------------------------------------
	void OGLVertexBuffer::Bind() const
	{
		KS_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}

	void OGLVertexBuffer::Unbind() const
	{
		KS_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	

	// ----------------------- Getters/Setters ------------------------------------------------------------
	void OGLVertexBuffer::SetData(const void* data, uint size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}



	// ---------------------------- INDEX BUFFER ----------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	OGLIndexBuffer::OGLIndexBuffer(uint* vertices, uint count)
		: m_Count(count)
	{
		KS_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_BufferID);
		
		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint), vertices, GL_STATIC_DRAW);
	}

	OGLIndexBuffer::~OGLIndexBuffer()
	{
		KS_PROFILE_FUNCTION();
		glDeleteBuffers(1, &m_BufferID);
	}

	

	// ----------------------- Public Index Buffer Methods ------------------------------------------------
	void OGLIndexBuffer::Bind() const
	{
		KS_PROFILE_FUNCTION();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	}

	void OGLIndexBuffer::Unbind() const
	{
		KS_PROFILE_FUNCTION();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}



	// ---------------------------- VERTEX ARRAY ----------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	OGLVertexArray::OGLVertexArray()
	{
		KS_PROFILE_FUNCTION();
		glCreateVertexArrays(1, &m_VArrayID);
	}

	OGLVertexArray::~OGLVertexArray()
	{
		KS_PROFILE_FUNCTION();
		glDeleteVertexArrays(1, &m_VArrayID);
	}


	
	// ----------------------- Public Vertex Array Methods ------------------------------------------------
	void OGLVertexArray::Bind() const
	{
		KS_PROFILE_FUNCTION();
		glBindVertexArray(m_VArrayID);
	}

	void OGLVertexArray::Unbind() const
	{
		KS_PROFILE_FUNCTION();
		glBindVertexArray(0);
	}


	void OGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertex_buffer)
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


	void OGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& index_buffer)
	{
		KS_PROFILE_FUNCTION();
		glBindVertexArray(m_VArrayID);
		index_buffer->Bind();

		m_IndexBuffer = index_buffer;
	}
}
