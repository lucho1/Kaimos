#ifndef _OPENGLTEXTURE_H_
#define _OPENGLTEXTURE_H_

#include "Renderer/Resources/Texture.h"
#include <glad/glad.h>

namespace Kaimos {

	class OpenGLTexture2D : public Texture2D
	{
	public:

		OpenGLTexture2D(uint width, uint height);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual uint GetWidth() const override { return m_Width; }
		virtual uint GetHeight() const override { return m_Height; }
		virtual uint GetTextureID() const override { return m_ID; }

		virtual void SetData(void* data, uint size) override;

		virtual void Bind(uint slot = 0) const override;

	private:

		std::string m_Path; // This is not 100% necessary, but OK for debugging... However shouldn't be here, there should be an "AssetManager" with a map storing [resource, path]
		uint m_Height = 0, m_Width = 0;
		uint m_ID = 0;

		GLenum m_InternalFormat = 0, m_DataFormat = 0;
	};

}
#endif