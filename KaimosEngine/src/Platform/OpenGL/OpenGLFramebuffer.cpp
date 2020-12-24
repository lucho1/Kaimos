#include "kspch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace Kaimos {

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSettings& settings) : m_FBOSettings(settings)
	{
		Resize(settings.width, settings.height);
	}
	
	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_ID);
		glDeleteTextures(1, &m_ColorTexture);
		glDeleteTextures(1, &m_DepthTexture);
	}


	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
		glViewport(0, 0, m_FBOSettings.width, m_FBOSettings.height);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint width, uint height)
	{
		if (m_ID != 0)
		{
			glDeleteFramebuffers(1, &m_ID);
			glDeleteTextures(1, &m_ColorTexture);
			glDeleteTextures(1, &m_DepthTexture);
		}

		m_FBOSettings.width = width;
		m_FBOSettings.height = height;


		// --- Create FBO ---
		glCreateFramebuffers(1, &m_ID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

		// --- Create Colored Texture ---
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorTexture);
		glBindTexture(GL_TEXTURE_2D, m_ColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Set MIN Filter for colored texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Set MAG Filter for colored texture

		// --- Create Depth Texture ---
		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthTexture);
		glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
		//glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_FBOSettings.width, m_FBOSettings.height);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

		// --- Attach Textures to FBO ---
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTexture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);

		// --- Unbind FBO ---
		// TODO: Assertions are not working, in this case, whether FBO is complete or not, it won't assert, and if I put a != instead, it will assert if it's complete
		KS_ENGINE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer Incompleted!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}