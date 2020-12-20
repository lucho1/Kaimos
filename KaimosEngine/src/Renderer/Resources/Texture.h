#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Core/Core.h"

namespace Kaimos {

	class Texture
	{
	public:

		virtual ~Texture() = default;

		virtual uint GetWidth() const = 0;
		virtual uint GetHeight() const = 0;
		virtual uint GetTextureID() const = 0;

		virtual void SetData(void* data, uint size) = 0;
		virtual void Bind(uint slot = 0) const = 0;

		virtual bool operator==(const Texture& texture) const = 0;
	};

	class Texture2D : public Texture
	{
	public: //Try including string?
		static Ref<Texture2D> Create(const std::string& path);	//TODO/OJU: We might want to create textures from other things (colors, gradients...)
		static Ref<Texture2D> Create(uint width, uint height);
	};

}
#endif