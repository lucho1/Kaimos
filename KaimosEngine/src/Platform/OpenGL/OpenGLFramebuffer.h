#ifndef _OPENGLFRAMEBUFFER_H_
#define _OPENGLFRAMEBUFFER_H_

#include "Renderer/Resources/Framebuffer.h"

namespace Kaimos {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:

		OpenGLFramebuffer(const FramebufferSettings& settings);
		virtual ~OpenGLFramebuffer();

		virtual void Resize(uint width, uint height) override;
		virtual void Bind() override;
		virtual void Unbind() override;


		virtual const FramebufferSettings& GetFBOSettings() const override { return m_FBOSettings; }
		virtual const uint GetFBOTextureID() const override { return m_ColorTexture; }

	private:

		uint m_ID = 0;
		uint m_ColorTexture = 0, m_DepthTexture = 0;
		FramebufferSettings m_FBOSettings;
	};

}
#endif //_OPENGL_FRAMEBUFFER_H_