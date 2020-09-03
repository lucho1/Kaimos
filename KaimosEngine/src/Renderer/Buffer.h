#ifndef _BUFFER_H_
#define _BUFFER_H_

// Pure virtual interface for different buffers for the different Rendering APIs (VTable?) -- No constructors
namespace Kaimos {

	enum class ShaderDataType { None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool };

	static uint ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:		return 4;			// sizeof(float)
			case ShaderDataType::Float2:	return 4 * 2;		// sizeof(float) * 2 ... and so on
			case ShaderDataType::Float3:	return 4 * 3;
			case ShaderDataType::Float4:	return 4 * 4;
			case ShaderDataType::Mat3:		return 4 * 3 * 3;	// same here sizeof(float) * 3 * 3 --> Matrix of 3x3
			case ShaderDataType::Mat4:		return 4 * 4 * 4;
			case ShaderDataType::Int:		return 4;
			case ShaderDataType::Int2:		return 4 * 2;
			case ShaderDataType::Int3:		return 4 * 3;
			case ShaderDataType::Int4:		return 4 * 4;
			case ShaderDataType::Bool:		return 1;			// sizeof(bool)
		}

		KS_ENGINE_ASSERT(false, "Unknown ShaderDataType passed!");
		return 0;
	}


	struct BufferElement
	{
		// -- Variables --
		std::string Name = "";
		ShaderDataType Type = ShaderDataType::None;
		uint Offset = 0, Size = 0;
		bool Normalized = false;

		// -- Functions --
		BufferElement() {}

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Normalized(normalized)
		{
		}

		uint GetElementTypeCount() const
		{
			switch (Type)
			{
				case ShaderDataType::Float:		return 1;
				case ShaderDataType::Float2:	return 2;
				case ShaderDataType::Float3:	return 3;
				case ShaderDataType::Float4:	return 4;
				case ShaderDataType::Mat3:		return 3 * 3;
				case ShaderDataType::Mat4:		return 4 * 4;
				case ShaderDataType::Int:		return 1;
				case ShaderDataType::Int2:		return 2;
				case ShaderDataType::Int3:		return 3;
				case ShaderDataType::Int4:		return 4;
				case ShaderDataType::Bool:		return 1;
			}

			KS_ENGINE_ASSERT(false, "The element has an unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:

		// The initializer list allows us to initialize this as
		//		BufferLayout layout = {{ ShaderDataType::Float3, "a_Position" }, ...} -- OR -- BufferLayout layout = BufferLayout({{ ShaderDataType::Float3, "a_Position" }, ...})
		// without having to declare it as a std::vector<BufferElement> layout and having a constructor such as BufferLayout(std::vector<BufferElement>)
		// so, is way more confortable and readable to have it as this, with an initializer list.
		BufferLayout(const std::initializer_list<BufferElement>& elements) : m_Elements(elements) { CalculateOffsetAndStride(); }
		BufferLayout() {}

		// -- Getters --
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
		inline const uint GetStride() const { return m_Stride; }

		// -- Elements Vector Iterators Getters --
		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:

		void CalculateOffsetAndStride()
		{
			uint offset = 0;
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


	class VertexBuffer
	{
	public:

		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		static VertexBuffer* Create(float* vertices, uint size); // This is the "Constructor", we take anything we want here (static cause doesn't belong to this class)
	};


	class IndexBuffer
	{
	public:

		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static IndexBuffer* Create(uint* vertices, uint count); // This is the "Constructor", we take anything we want here (static cause doesn't belong to this class)
		
		// -- Getters --
		virtual uint GetCount() const = 0;
	};


	class VertexArray
	{
	public:

		virtual ~VertexArray() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& Vbuffer) = 0;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& Ibuffer) = 0;

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const = 0;

		static VertexArray* Create(); // This is the "Constructor", we take anything we want here (static cause doesn't belong to this class)
	};

}


#endif