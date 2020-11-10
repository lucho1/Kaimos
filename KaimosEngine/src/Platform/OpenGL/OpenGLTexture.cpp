#include "kspch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"
#include <glad/glad.h>

namespace Kaimos {

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
	{
		int w, h, channels;
		stbi_set_flip_vertically_on_load(1);
		
		stbi_uc* texture_data = stbi_load(path.c_str(), &w, &h, &channels, 0);
		KS_ENGINE_ASSERT(texture_data, "Failed to load texture data from path: {0}", path.c_str());

		m_Width = w;
		m_Height = h;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

		// To have mipmaps, modify "levels" parameter, and to work with gamma and all that stuff, the "internalFormat" parameter (GL_SRGBA8) --> GL_RGB8 = image RGBA with 8 bits per channel (8b R, 8b G, 8b B)
		glTextureStorage2D(m_ID, 1, GL_RGBA8, m_Width, m_Height);			// Allocate memory in GPU for the texture

		// --- Texture Parameters Setup ---
		// Texture filters to minificate and magnificate textures when they are smaller than geometry's pixels to fill
		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		// GL_LINEAR will make the minification (zoomed-out) of the texture to be linearly interpolated to the color we want
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);		// GL_NEAREST will make the magnification (zoomed-in) to snap into the nearest pixel (instead of blurring with GL_LINEAR) --> Linear filtering is OK for images but not with few colors

		glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, texture_data); // X,Y Offset can be use to upload partially a texture, you can change a region of an already uploaded texture
		
		stbi_image_free(texture_data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_ID);
	}

	void OpenGLTexture2D::Bind(uint slot) const
	{
		glBindTextureUnit(slot, m_ID); //Slot/Unit refers to the (opengl) slot in which the texture is bound, in case we bind +1 textures at a time
	}

}