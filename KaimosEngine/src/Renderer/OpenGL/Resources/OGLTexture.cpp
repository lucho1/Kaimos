#include "kspch.h"
#include "OGLTexture.h"

#include <stb_image.h>

namespace Kaimos {

	// ----------------------- TEXTURE 2D -----------------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	OGLTexture2D::OGLTexture2D(uint width, uint height)
	{
		KS_PROFILE_FUNCTION();
		m_Width = width;
		m_Height = height;
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

	OGLTexture2D::OGLTexture2D(const std::string& filepath)
	{
		// -- Check Paths --
		// In this case we check "assets" for textures and "internal" for icons
		if (filepath.find("assets") == std::string::npos && filepath.find("internal") == std::string::npos)
		{
			KS_ERROR("Cannot load an out-of-project texture! Try moving it inside 'assets/' folder.\nCurrent Filepath: {0}", filepath);
			return;
		}
		
		std::filesystem::path path = filepath;
		if (!std::filesystem::exists(path))
		{
			KS_ERROR("Unexisting Path Loading Texture: {0}", filepath);
			return;
		}

		// -- Texture Load --
		KS_PROFILE_FUNCTION();
		int w, h, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* texture_data = nullptr;

		{
			KS_PROFILE_SCOPE("TEXTURE STBI LOAD - OGLTexture2D::OGLTexture2D(const std::string& path)");
			texture_data = stbi_load(filepath.c_str(), &w, &h, &channels, 0);
		}

		// -- Check for Failure --
		if (!texture_data)
		{
			KS_ERROR("Failed to load texture data from path: {0}", filepath);
			return;
		}

		m_Width = w; m_Height = h, m_Filepath = filepath;

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
		else if (channels == 1)
		{
			m_InternalFormat = GL_R8;
			m_DataFormat = GL_RED;
		}

		KS_ENGINE_ASSERT(m_InternalFormat & m_DataFormat, "Image Format not Supported!"); // It'll be false (0) if either of them is 0 (what we want)

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

	OGLTexture2D::~OGLTexture2D()
	{
		KS_PROFILE_FUNCTION();
		glDeleteTextures(1, &m_ID);
	}

	
	// ----------------------- Public Texture Methods -----------------------------------------------------
	void OGLTexture2D::SetData(void* data, uint size)
	{
		KS_PROFILE_FUNCTION();

		uint bpp = m_DataFormat == GL_RGBA ? 4 : 3; // Bytes per pixel
		KS_ENGINE_ASSERT(size == m_Width * m_Height * bpp, "Data passed must be the same size than the entire texture size");
		glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OGLTexture2D::Bind(uint slot) const
	{
		KS_PROFILE_FUNCTION();
		glBindTextureUnit(slot, m_ID); //Slot/Unit refers to the (opengl) slot in which the texture is bound, in case we bind +1 textures at a time
	}




	// ----------------------- HDR TEXTURE ----------------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	OGL_HDRTexture2D::OGL_HDRTexture2D(const std::string& filepath)
	{
		// -- Check Paths --
		if (filepath.find("assets") == std::string::npos)
		{
			KS_ERROR("Cannot load an out-of-project texture! Try moving it inside 'assets/' folder.\nCurrent Filepath: {0}", filepath);
			return;
		}

		// In this case we check also the extension
		std::filesystem::path path = filepath;
		if (!std::filesystem::exists(path) || (path.extension() != ".hdr"))
		{
			KS_ERROR("Unexisting or Invalid Path or Extension Loading HDR Texture: {0}", filepath);
			return;
		}

		// -- Texture Load --
		KS_PROFILE_FUNCTION();
		int w, h, channels;
		stbi_set_flip_vertically_on_load(1);
		float* texture_data = nullptr;

		{
			KS_PROFILE_SCOPE("HDR TEXTURE STBI LOAD - OGL_HDRTexture2D::OGL_HDRTexture2D(const std::string& path)");
			texture_data = stbi_loadf(filepath.c_str(), &w, &h, &channels, 0);
		}

		// -- Check for Failure --
		if (!texture_data)
		{
			KS_ERROR("Failed to load HDR texture data from path: {0}", filepath);
			return;
		}

		// -- Texture Creation --
		m_Width = w; m_Height = h, m_Filepath = filepath;
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
		glTextureStorage2D(m_ID, 1, GL_RGB16F, m_Width, m_Height);

		// --- Texture Parameters Setup ---
		// Texture filters to minificate and magnificate textures when they are smaller than geometry's pixels to fill
		glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_FLOAT, texture_data);

		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		stbi_image_free(texture_data);
	}

	OGL_HDRTexture2D::~OGL_HDRTexture2D()
	{
		KS_PROFILE_FUNCTION();
		glDeleteTextures(1, &m_ID);
	}


	// ----------------------- Public Texture Methods -----------------------------------------------------
	void OGL_HDRTexture2D::Bind(uint slot) const
	{
		KS_PROFILE_FUNCTION();
		glBindTextureUnit(slot, m_ID);
	}




	// ----------------------- HDR TEXTURE ----------------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	OGL_LUTTexture::OGL_LUTTexture(uint size)
	{
		// -- Texture Creation --
		m_Width = m_Height = size;
		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, size, size, 0, GL_RG, GL_FLOAT, 0);

		// --- Texture Parameters Setup ---
		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	void OGL_LUTTexture::Bind(uint slot) const
	{
		KS_PROFILE_FUNCTION();
		glBindTextureUnit(slot, m_ID);
	}




	// ----------------------- CUBEMAP TEXTURE ------------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	OGL_CubemapTexture::OGL_CubemapTexture(uint width, uint height, bool linear_mipmap_filtering)
	{
		// -- Texture Creation --
		m_Width = width;
		m_Height = height;

		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);

		for (uint i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, nullptr);

		// --- Texture Parameters Setup ---
		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, linear_mipmap_filtering ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	OGL_CubemapTexture::~OGL_CubemapTexture()
	{
		KS_PROFILE_FUNCTION();
		glDeleteTextures(1, &m_ID);
	}

	void OGL_CubemapTexture::Bind(uint slot) const
	{
		KS_PROFILE_FUNCTION();
		glBindTextureUnit(slot, m_ID);
	}

	void OGL_CubemapTexture::GenerateMipMap() const
	{
		KS_PROFILE_FUNCTION();
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
}
