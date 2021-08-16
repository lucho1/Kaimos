#include "kspch.h"
#include "OGLFramebuffer.h"


namespace Kaimos {

	// ----------------------- Globals --------------------------------------------------------------------
	static const uint s_MaxFBOSize = 8192; // TODO: Don't hardcode this, this should be based on render capabilities
	
	static GLenum GLTextureFormat(TEXTURE_FORMAT format)
	{
		switch (format)
		{
			case TEXTURE_FORMAT::DEPTH24STENCIL8:	return GL_DEPTH24_STENCIL8;
			case TEXTURE_FORMAT::RED_INTEGER:		return GL_RED_INTEGER;
			case TEXTURE_FORMAT::RGBA8:				return GL_RGBA8;
		}

		KS_FATAL_ERROR("Invalid Format Passed!");
		return GL_NONE;
	}

	static bool IsDepthFormatTexture(TEXTURE_FORMAT format)
	{
		switch (format)
		{
			case TEXTURE_FORMAT::DEPTH24STENCIL8: return true;
		}

		return false;
	}


	
	// ----------------------- Public Class Methods -------------------------------------------------------
	OGLFramebuffer::OGLFramebuffer(const FramebufferSettings& settings, bool generate_depth_renderbuffer) : m_FBOSettings(settings)
	{
		KS_PROFILE_FUNCTION();
		for (FramebufferTextureSettings settings : m_FBOSettings.FBOAttachments.TextureAttachments)
		{
			if (IsDepthFormatTexture(settings.TextureFormat))
				m_DepthAttachmentSetting = settings;
			else
				m_ColorAttachmentSettings.push_back(settings);
		}

		Resize(settings.Width, settings.Height, generate_depth_renderbuffer);
	}

	OGLFramebuffer::OGLFramebuffer(uint width, uint height, bool generate_depth_renderbuffer)
	{
		FramebufferSettings settings;
		settings.Width = width;
		settings.Height = height;

		m_FBOSettings = settings;
		Generate(generate_depth_renderbuffer);
	}
	
	OGLFramebuffer::~OGLFramebuffer()
	{
		KS_PROFILE_FUNCTION();
		glDeleteFramebuffers(1, &m_ID);
		glDeleteTextures(m_ColorTextures.size(), m_ColorTextures.data());
		glDeleteTextures(1, &m_DepthTexture);
	}


	
	// ----------------------- Public FBO Methods ---------------------------------------------------------
	void OGLFramebuffer::Bind()
	{
		KS_PROFILE_FUNCTION();
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
		glViewport(0, 0, m_FBOSettings.Width, m_FBOSettings.Height);
	}

	void OGLFramebuffer::Unbind()
	{
		KS_PROFILE_FUNCTION();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OGLFramebuffer::ClearFBOTexture(uint index, int value)
	{
		KS_PROFILE_FUNCTION();
		KS_ENGINE_ASSERT(index < m_ColorTextures.size(), "FBO: Index out of bounds");
		glClearTexImage(m_ColorTextures[index], 0, GLTextureFormat(m_ColorAttachmentSettings[index].TextureFormat), GL_INT, &value);
	}

	void OGLFramebuffer::Resize(uint width, uint height, bool generate_depth_renderbuffer)
	{
		KS_PROFILE_FUNCTION();
		if (width == 0 || height == 0 || width > s_MaxFBOSize || height > s_MaxFBOSize)
		{
			KS_ENGINE_WARN("Warning: Tried to resize FBO to {0}x{1}, aborting operation", width, height);
			return;
		}

		m_FBOSettings.Width = width;
		m_FBOSettings.Height = height;
		Generate(generate_depth_renderbuffer);

		// -- FBO Color Attachments --
		GLenum FBOsampling = m_FBOSettings.Samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		if (m_ColorAttachmentSettings.size() > 0)
		{
			m_ColorTextures.resize(m_ColorAttachmentSettings.size());
			glCreateTextures(FBOsampling, m_ColorTextures.size(), m_ColorTextures.data());

			for (size_t i = 0; i < m_ColorTextures.size(); ++i)
			{
				glBindTexture(FBOsampling, m_ColorTextures[i]);
				switch (m_ColorAttachmentSettings[i].TextureFormat)
				{
					case TEXTURE_FORMAT::RGBA8:
						SetTexture(false, GL_RGBA8, GL_RGBA, m_FBOSettings.Width, m_FBOSettings.Height, m_FBOSettings.Samples);
						break;
					case TEXTURE_FORMAT::RED_INTEGER:
						SetTexture(false, GL_R32I, GL_RED_INTEGER, m_FBOSettings.Width, m_FBOSettings.Height, m_FBOSettings.Samples);
						break;
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, FBOsampling, m_ColorTextures[i], 0);
			}
		}

		// -- FBO Depth Attachment --
		if (m_DepthAttachmentSetting.TextureFormat != TEXTURE_FORMAT::NONE)
		{
			glCreateTextures(FBOsampling, 1, &m_DepthTexture);
			glBindTexture(FBOsampling, m_DepthTexture);
			switch (m_DepthAttachmentSetting.TextureFormat)
			{
				case TEXTURE_FORMAT::DEPTH24STENCIL8:
					SetTexture(true, GL_DEPTH24_STENCIL8, GL_NONE, m_FBOSettings.Width, m_FBOSettings.Height, m_FBOSettings.Samples);
					break;
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, FBOsampling, m_DepthTexture, 0);
		}

		// -- Draw Buffers --
		if (m_ColorTextures.size() > 1)
		{
			KS_ENGINE_ASSERT(m_ColorTextures.size() <= 4, "Kaimos doesn't allows more than 4 color attachments!");

			GLenum color_buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers((GLsizei)m_ColorTextures.size(), color_buffers);
		}
		else if (m_ColorTextures.empty()) // Depth pass
			glDrawBuffer(GL_NONE);

		// -- Unbind FBO --
		bool fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
		KS_ENGINE_ASSERT(fbo_status, "Framebuffer Incompleted!");

		if (!fbo_status)
			KS_ENGINE_WARN("Framebuffer Incompleted");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	
	// ----------------------- Getters --------------------------------------------------------------------
	uint OGLFramebuffer::GetFBOTextureID(uint index) const
	{
		KS_ENGINE_ASSERT(index < m_ColorTextures.size(), "FBO - Index is outside bounds");
		return m_ColorTextures[index];
	}

	int OGLFramebuffer::GetPixelFromFBO(uint index, int x, int y)
	{
		KS_PROFILE_FUNCTION();
		KS_ENGINE_ASSERT(index < m_ColorTextures.size(), "FBO - Index is outside bounds");

		// -- Select a buffer for reading (a specific color attachment) --
		glReadBuffer(GL_COLOR_ATTACHMENT0 + index);

		// -- Read pixels from FBO (x, y with a 1,1 size because we want to read 1 pixel) --
		int pixel;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel);
		return pixel;
	}



	// ----------------------- Private FBO Methods --------------------------------------------------------
	void OGLFramebuffer::Generate(bool generate_depth_renderbuffer)
	{
		// -- Delete Previous FBO (if any) --
		KS_PROFILE_FUNCTION();
		if (m_ID != 0)
		{
			glDeleteFramebuffers(1, &m_ID);
			glDeleteTextures(m_ColorTextures.size(), m_ColorTextures.data());
			glDeleteTextures(1, &m_DepthTexture);
			m_ColorTextures.clear();
			m_DepthTexture = 0;
		}		

		// -- Create FBO --
		glCreateFramebuffers(1, &m_ID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

		if (generate_depth_renderbuffer)
		{
			glCreateRenderbuffers(1, &m_RBOID);
			glBindRenderbuffer(GL_RENDERBUFFER, m_RBOID);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_FBOSettings.Width, m_FBOSettings.Height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RBOID);
		}
	}


	void OGLFramebuffer::SetTexture(bool depth_texture, GLenum internal_format, GLenum format, uint width, uint height, uint samples)
	{
		KS_PROFILE_FUNCTION();
		if (samples > 1)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format, width, height, GL_FALSE);
			return;
		}

		if (!depth_texture)
			glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
		else
			glTexStorage2D(GL_TEXTURE_2D, 1, internal_format, width, height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		// Set MIN/MAG Filter for texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);	// Set Wraps for texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
}
