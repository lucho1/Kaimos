#ifndef _OPENGLTEXTURE_H_
#define _OPENGLTEXTURE_H_

#include "Renderer/Resources/Texture.h"
#include <glad/glad.h>

namespace Kaimos {

	class OGLTexture2D : public Texture2D
	{
	public:

		// --- Public Class Methods ---
		OGLTexture2D(uint width, uint height);
		OGLTexture2D(const std::string& filepath);
		virtual ~OGLTexture2D();

		// --- Public Texture Methods ---
		virtual void SetData(void* data, uint size)	override;
		virtual void Bind(uint slot = 0)			const override;

		// --- Getters ---
		virtual const std::string& GetFilepath()	const override { return m_Filepath; }

	private:

		std::string m_Filepath = ""; // TODO: This is not 100% necessary, but OK for debugging... However shouldn't be here, there should be an "AssetManager" with a map storing [resource, path]
		GLenum m_InternalFormat = 0, m_DataFormat = 0;
	};



	class OGL_HDRTexture2D : public HDRTexture2D
	{
	public:

		// --- Public Class Methods ---
		OGL_HDRTexture2D(const std::string& filepath);
		virtual ~OGL_HDRTexture2D();

		// --- Public Texture Methods ---
		virtual void Bind(uint slot = 0)			const override;
		virtual const std::string& GetFilepath()	const override { return m_Filepath; }

	private:

		std::string m_Filepath = "";
	};



	class OGL_CubemapTexture : public CubemapTexture
	{
	public:
		OGL_CubemapTexture(uint width, uint height, bool linear_mipmap_filtering = false);
		virtual ~OGL_CubemapTexture();

		virtual void Bind(uint slot = 0) const override;
		virtual void GenerateMipMap() const override;
	};
}

#endif //_OPENGLTEXTURE_H_
