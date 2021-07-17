#ifndef _BUFFER_H_
#define _BUFFER_H_


// --- Pure virtual interface for different buffers for the different Rendering APIs (VTable?) - No constructors ---
namespace Kaimos {


	enum class SHADER_DATATYPE { NONE = 0, FLOAT, FLOAT2, FLOAT3, FLOAT4, MAT3, MAT4, INT, INT2, INT3, INT4, BOOL };


	static uint ShaderDataTypeSize(SHADER_DATATYPE type)
	{
		switch (type)
		{
			case SHADER_DATATYPE::FLOAT:	return 4;			// sizeof(float)
			case SHADER_DATATYPE::FLOAT2:	return 4 * 2;		// sizeof(float) * 2 ... and so on
			case SHADER_DATATYPE::FLOAT3:	return 4 * 3;
			case SHADER_DATATYPE::FLOAT4:	return 4 * 4;
			case SHADER_DATATYPE::MAT3:		return 4 * 3 * 3;	// same here sizeof(float) * 3 * 3 --> Matrix of 3x3
			case SHADER_DATATYPE::MAT4:		return 4 * 4 * 4;
			case SHADER_DATATYPE::INT:		return 4;
			case SHADER_DATATYPE::INT2:		return 4 * 2;
			case SHADER_DATATYPE::INT3:		return 4 * 3;
			case SHADER_DATATYPE::INT4:		return 4 * 4;
			case SHADER_DATATYPE::BOOL:		return 1;			// sizeof(bool)
		}

		KS_FATAL_ERROR("Unknown ShaderDataType passed!");
		return 0;
	}



	// ---- Elements of a Buffer struct definition ----
	struct BufferElement
	{
		// --- Variables ---
		std::string Name = "";
		SHADER_DATATYPE Type = SHADER_DATATYPE::NONE;
		size_t Offset = 0;
		uint Size = 0;
		bool Normalized = false;

		// --- Functions ---
		BufferElement() = default;

		BufferElement(SHADER_DATATYPE type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Normalized(normalized)
		{
		}

		uint GetElementTypeCount() const
		{
			switch (Type)
			{
				case SHADER_DATATYPE::FLOAT:	return 1;
				case SHADER_DATATYPE::FLOAT2:	return 2;
				case SHADER_DATATYPE::FLOAT3:	return 3;
				case SHADER_DATATYPE::FLOAT4:	return 4;
				case SHADER_DATATYPE::MAT3:		return 3 * 3;
				case SHADER_DATATYPE::MAT4:		return 4 * 4;
				case SHADER_DATATYPE::INT:		return 1;
				case SHADER_DATATYPE::INT2:		return 2;
				case SHADER_DATATYPE::INT3:		return 3;
				case SHADER_DATATYPE::INT4:		return 4;
				case SHADER_DATATYPE::BOOL:		return 1;
			}

			KS_FATAL_ERROR("The element has an unknown ShaderDataType!");
			return 0;
		}
	};



	// ---- Class to define a Buffer Layout ----
	class BufferLayout
	{
	public:

		// --- Public Class Methods ---
		// The initializer list allows us to initialize as:	BufferLayout layout = {{ ShaderDataType::Float3, "a_Position" }, ...}
		// without declaring it as a std::vector<BufferElement> and having a constructor such as BufferLayout(std::vector<BufferElement>).
		BufferLayout(std::initializer_list<BufferElement> elements) : m_Elements(elements) { CalculateOffsetAndStride(); }
		BufferLayout() = default;

		// -- Getters --
		inline const std::vector<BufferElement>& GetElements()	const	{ return m_Elements; }
		inline const uint GetStride()							const	{ return m_Stride; }

		// -- Iterators --
		std::vector<BufferElement>::const_iterator begin()		const	{ return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end()		const	{ return m_Elements.end(); }
		std::vector<BufferElement>::iterator begin()					{ return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end()						{ return m_Elements.end(); }

	private:

		// --- Private Class Methods ---
		void CalculateOffsetAndStride()
		{
			size_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

	private:

		std::vector<BufferElement> m_Elements;
		uint m_Stride = 0;
	};



	// ---- Class to define a Vertex Buffer (virtual interface) ----
	class VertexBuffer
	{
	public:

		// --- Public Class Methods ---
		virtual ~VertexBuffer() = default;

		// --- Public Vertex Buffer Methods ---
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Ref<VertexBuffer> Create(float* vertices, uint size); // This is the "Class Constructor", we take anything we want here (static because doesn't belong to this class)
		static Ref<VertexBuffer> Create(uint size);

		// --- Getters/Setters ---
		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual void SetData(const void* data, uint size) = 0;
	};



	// ---- Class to define an Index Buffer (virtual interface) ----
	class IndexBuffer
	{
	public:

		// --- Public Class Methods ---
		virtual ~IndexBuffer() = default;

		// --- Public Index Buffer Methods ---
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Ref <IndexBuffer> Create(uint* vertices, uint count);
		static Ref <IndexBuffer> Create(uint count);
		
		// -- Getters --
		virtual uint GetCount() const = 0;
		virtual void SetData(const void* data, uint count) = 0;
	};



	// ---- Class to define a Vertex Array (virtual interface) ----
	class VertexArray
	{
	public:

		// --- Public Class Methods ---
		virtual ~VertexArray() = default;

		// --- Public Vertex Array Methods ---
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& Vbuffer) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& Ibuffer) = 0;
		
		static Ref<VertexArray> Create();

		// --- Getters ---
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;
		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
	};
}

#endif //_BUFFER_H_
