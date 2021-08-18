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
		virtual void Bind(uint slot = 0) const = 0;

		// --- Getters ---
		uint GetWidth()		const { return m_Width; }
		uint GetHeight()	const { return m_Height; }
		uint GetTextureID()	const { return m_ID; }

		// --- Operators ---
		bool operator==(const Texture& texture) const { return m_ID == ((Texture&)texture).m_ID; }

	protected:
		uint m_Height = 0, m_Width = 0;
		uint m_ID = 0;
	};



	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const std::string& filepath);	//TODO/OJU: We might want to create textures from other things (colors, gradients...)
		static Ref<Texture2D> Create(uint width, uint height);

		virtual void SetData(void* data, uint size) = 0;
		virtual const std::string& GetFilepath() const = 0;
	};


	class HDRTexture2D : public Texture
	{
	public:
		static Ref<HDRTexture2D> Create(const std::string& filepath);

		virtual const std::string& GetFilepath() const = 0;
	};


	class CubemapTexture : public Texture
	{
	public:
		static Ref<CubemapTexture> Create(uint width, uint height, bool linear_mipmap_filtering = false);

		virtual void GenerateMipMap() const = 0;
	};
}

#endif //_TEXTURE_H_
