#ifndef _OPENGLFRAMEBUFFER_H_
#define _OPENGLFRAMEBUFFER_H_

#include "Renderer/Resources/Framebuffer.h"
#include <glad/glad.h>


namespace Kaimos {

	class OGLFramebuffer : public Framebuffer
	{
	public:

		// --- Public Class Methods ---
		OGLFramebuffer(const FramebufferSettings& settings, bool generate_depth_renderbuffer);
		OGLFramebuffer(uint width, uint height, bool generate_depth_renderbuffer);
		virtual ~OGLFramebuffer();

		// --- Public FBO Methods ---
		virtual void Bind() override;
		virtual void Unbind() override;
		
		virtual void Resize(uint width, uint height, bool generate_depth_renderbuffer = false) override;
		virtual void ClearFBOTexture(uint index, int value) override;

	public:

		// --- Getters ---
		virtual int GetPixelFromFBO(uint index, int x, int y) override;
		virtual uint GetFBOTextureID(uint index = 0) const override;

		virtual const FramebufferSettings& GetFBOSettings() const override { return m_FBOSettings; }		

	private:

		// --- Private FBO Methods ---
		void Generate(bool generate_depth_renderbuffer);
		void SetTexture(bool depth_texture, GLenum internal_format, GLenum format, uint width, uint height, uint samples);

	private:

		uint m_ID = 0, m_RBOID = 0;
		FramebufferSettings m_FBOSettings;

		// --- Attachments ---
		std::vector<FramebufferTextureSettings> m_ColorAttachmentSettings;
		std::vector<uint> m_ColorTextures;

		FramebufferTextureSettings m_DepthAttachmentSetting;
		uint m_DepthTexture = 0;
	};
}

#endif //_OPENGL_FRAMEBUFFER_H_
