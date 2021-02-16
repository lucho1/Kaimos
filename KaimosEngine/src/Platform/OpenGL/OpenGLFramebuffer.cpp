#include "kspch.h"
#include "OpenGLFramebuffer.h"


namespace Kaimos {

	static const uint s_MaxFBOSize = 8192; // TODO: Don't hardcode this, this should be based on render capabilities

	static bool IsDepthFormatTexture(TEXTURE_FORMAT format)
	{
		switch (format)
		{
			case TEXTURE_FORMAT::DEPTH24STENCIL8:	return true;
		}

		return false;
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSettings& settings) : m_FBOSettings(settings)
	{
		for (FramebufferTextureSettings settings : m_FBOSettings.FBOAttachments.TextureAttachments)
		{
			if (IsDepthFormatTexture(settings.TextureFormat))
				m_DepthAttachmentSetting = settings;
			else
				m_ColorAttachmentSettings.push_back(settings);
		}

		Resize(settings.Width, settings.Height);
	}
	
	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_ID);
		glDeleteTextures(m_ColorTextures.size(), m_ColorTextures.data());
		glDeleteTextures(1, &m_DepthTexture);
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
		glViewport(0, 0, m_FBOSettings.Width, m_FBOSettings.Height);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	void OpenGLFramebuffer::Resize(uint width, uint height)
	{
		if (width == 0 || height == 0 || width > s_MaxFBOSize || height > s_MaxFBOSize)
		{
			KS_ENGINE_WARN("Warning: Tried to resize FBO to {0}x{1}, aborting operation", width, height);
			return;
		}

		if (m_ID != 0)
		{
			glDeleteFramebuffers(1, &m_ID);
			glDeleteTextures(m_ColorTextures.size(), m_ColorTextures.data());
			glDeleteTextures(1, &m_DepthTexture);

			m_ColorTextures.clear();
			m_DepthTexture = 0;
		}

		m_FBOSettings.Width = width;
		m_FBOSettings.Height = height;

		// --- Create FBO ---
		GLenum FBOsampling = m_FBOSettings.Samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		glCreateFramebuffers(1, &m_ID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

		// --- FBO Color Attachments ---
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
						SetTexture(false, GL_RGBA8, m_FBOSettings.Width, m_FBOSettings.Height, m_FBOSettings.Samples);
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, FBOsampling, m_ColorTextures[i], 0);
						break;
				}
			}
		}

		// --- FBO Depth Attachment ---
		if (m_DepthAttachmentSetting.TextureFormat != TEXTURE_FORMAT::NONE)
		{
			glCreateTextures(FBOsampling, 1, &m_DepthTexture);
			glBindTexture(FBOsampling, m_DepthTexture);
			switch (m_DepthAttachmentSetting.TextureFormat)
			{
				case TEXTURE_FORMAT::DEPTH24STENCIL8:
					SetTexture(true, GL_DEPTH24_STENCIL8, m_FBOSettings.Width, m_FBOSettings.Height, m_FBOSettings.Samples);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, FBOsampling, m_DepthTexture, 0);
					break;
			}
		}

		// --- Draw Buffers ---
		if (m_ColorTextures.size() > 1)
		{
			// TODO: Same old problem with assertions, this is not working, and it should be <= 4
			KS_ENGINE_ASSERT(m_ColorTextures.size() >= 4, "Kaimos doesn't allows more than 4 color attachments!");

			GLenum color_buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorTextures.size(), color_buffers);
		}
		else if (m_ColorTextures.empty())
			glDrawBuffer(GL_NONE); // Depth pass

		// --- Unbind FBO ---
		// TODO: Assertions are not working, in this case, whether FBO is complete or not, it won't assert, and if I put a != instead, it will assert if it's complete
		KS_ENGINE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == (bool)GL_FRAMEBUFFER_COMPLETE, "Framebuffer Incompleted!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	void OpenGLFramebuffer::SetTexture(bool depth_texture, GLenum format, uint width, uint height, uint samples)
	{
		if (samples > 1)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			return;
		}

		if (!depth_texture)
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		else
			glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		// Set MIN/MAG Filter for texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);	// Set Wraps for texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
}
