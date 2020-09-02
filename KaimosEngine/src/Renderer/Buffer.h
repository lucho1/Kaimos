#ifndef _BUFFER_H_
#define _BUFFER_H_

// Pure virtual interface for different buffers for the different Rendering APIs (VTable?) -- No constructors
namespace Kaimos {

	struct VertexData
	{

	};

	class VertexBuffer
	{
	public:

		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static VertexBuffer* Create(float* vertices, uint size); // This is the "Constructor", we take anything we want here (static cause doesn't belong to this class)
	};

	class IndexBuffer
	{
	public:

		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint GetCount() const = 0;

		static IndexBuffer* Create(uint* vertices, uint count); // This is the "Constructor", we take anything we want here (static cause doesn't belong to this class)
	};

}


#endif