#ifndef _OPENGLTEXTURE_H_
#define _OPENGLTEXTURE_H_

#include "Renderer/Resources/Texture.h"
#include <glad/glad.h>

namespace Kaimos {

	class OpenGLTexture2D : public Texture2D
	{
	public:

		// --- Public Class Methods ---
		OpenGLTexture2D(uint width, uint height);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		// --- Public Texture Methods ---
		virtual void SetData(void* data, uint size) override;
		virtual void Bind(uint slot = 0) const override;

		// --- Getters ---
		virtual uint GetWidth()		const override { return m_Width; }
		virtual uint GetHeight()	const override { return m_Height; }
		virtual uint GetTextureID()	const override { return m_ID; }

		// --- Operators ---
		virtual bool operator==(const Texture& texture) const override { return m_ID == ((OpenGLTexture2D&)texture).m_ID; }

	private:

		std::string m_Path = ""; // TODO: This is not 100% necessary, but OK for debugging... However shouldn't be here, there should be an "AssetManager" with a map storing [resource, path]
		uint m_Height = 0, m_Width = 0;
		uint m_ID = 0;

		GLenum m_InternalFormat = 0, m_DataFormat = 0;
	};

}

#endif //_OPENGLTEXTURE_H_
