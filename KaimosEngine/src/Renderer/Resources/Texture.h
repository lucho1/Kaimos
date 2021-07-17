#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Core/Core.h"

namespace Kaimos {

	class Texture
	{
	public:

		// --- Public Class Methods ---
		virtual ~Texture() = default;

		// --- Public Texture Methods ---
		virtual void SetData(void* data, uint size) = 0;
		virtual void Bind(uint slot = 0) const = 0;

		// --- Getters ---
		virtual uint GetWidth()		const = 0;
		virtual uint GetHeight()	const = 0;
		virtual uint GetTextureID()	const = 0;

		// --- Operators ---
		virtual bool operator==(const Texture& texture) const = 0;
	};



	class Texture2D : public Texture
	{
	public:
		// --- Public Texture2D Methods ---
		static Ref<Texture2D> Create(const std::string& filepath);	//TODO/OJU: We might want to create textures from other things (colors, gradients...)
		static Ref<Texture2D> Create(uint width, uint height);
	};
}

#endif //_TEXTURE_H_
