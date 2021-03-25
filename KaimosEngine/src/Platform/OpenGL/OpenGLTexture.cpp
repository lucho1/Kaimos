#include "kspch.h"
#include "OpenGLTexture.h"

#include <stb_image.h>

namespace Kaimos {

	// ----------------------- Public Class Methods -------------------------------------------------------
	OpenGLTexture2D::OpenGLTexture2D(uint width, uint height) : m_Width(width), m_Height(height)
	{
		KS_PROFILE_FUNCTION();
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		// -- Texture Creation --
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

		// -- Texture Storage --
		glTextureStorage2D(m_ID, 1, GL_RGBA8, m_Width, m_Height);

		// -- Texture Parameters Setup --
		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}


	OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : m_Path(path)
	{
		// -- Texture Load --
		KS_PROFILE_FUNCTION();
		int w, h, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* texture_data = nullptr;
		{
			KS_PROFILE_SCOPE("TEXTURE STBI LOAD - OpenGLTexture2D::OpenGLTexture2D(const std::string & path)");
			texture_data = stbi_load(path.c_str(), &w, &h, &channels, 0);
			KS_ENGINE_ASSERT(texture_data, "Failed to load texture data from path: {0}", path.c_str());
		}

		m_Width = w;
		m_Height = h;

		// -- Image channels (RGBA) processing --
		if (channels == 4)
		{
			m_InternalFormat = GL_RGBA8;
			m_DataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			m_InternalFormat = GL_RGB8;
			m_DataFormat = GL_RGB;
		}

		KS_ENGINE_ASSERT(m_InternalFormat & m_InternalFormat, "Image Format not Supported!"); // It'll be 0 if either of them is 0 (what we want)

		// -- Texture Creation --
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

		// -- For mipmaps, modify "levels" parameter. To work with gamma and all that stuff, the "internalFormat" parameter (GL_SRGBA8) --> GL_RGB8 = image RGBA with 8 bits per channel (8b R, 8b G, 8b B) --
		glTextureStorage2D(m_ID, 1, m_InternalFormat, m_Width, m_Height);	// Allocate memory in GPU for the texture

		// --- Texture Parameters Setup ---
		// Texture filters to minificate and magnificate textures when they are smaller than geometry's pixels to fill
		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		// GL_LINEAR will make the minification (zoomed-out) of the texture to be linearly interpolated to the color we want
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);		// GL_NEAREST will make the magnification (zoomed-in) to snap into the nearest pixel (instead of blurring with GL_LINEAR) --> Linear filtering is OK for images but not with few colors

		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, texture_data); // X,Y Offset can be use to upload partially a texture, you can change a region of an already uploaded texture
		
		// -- Free STBI Image --
		stbi_image_free(texture_data);
	}


	OpenGLTexture2D::~OpenGLTexture2D()
	{
		KS_PROFILE_FUNCTION();
		glDeleteTextures(1, &m_ID);
	}


	
	// ----------------------- Public Texture Methods -----------------------------------------------------
	void OpenGLTexture2D::SetData(void* data, uint size)
	{
		KS_PROFILE_FUNCTION();

		uint bpp = m_DataFormat == GL_RGBA ? 4 : 3; // Bytes per pixel
		KS_ENGINE_ASSERT(size == m_Width * m_Height * bpp, "Data passed must be the same size than the entire texture size");
		KS_ENGINE_ASSERT((m_DataFormat == GL_RGBA || m_DataFormat == GL_RGB), "Texture format was wrong");

		glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint slot) const
	{
		KS_PROFILE_FUNCTION();
		glBindTextureUnit(slot, m_ID); //Slot/Unit refers to the (opengl) slot in which the texture is bound, in case we bind +1 textures at a time
	}
}
