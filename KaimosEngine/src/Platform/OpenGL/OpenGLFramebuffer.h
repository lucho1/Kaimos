#ifndef _OPENGLFRAMEBUFFER_H_
#define _OPENGLFRAMEBUFFER_H_

#include "Renderer/Resources/Framebuffer.h"
#include <glad/glad.h>


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
		// TODO: Caíste en la vieja trampa del assert que no funciona :D
		virtual const uint GetFBOTextureID(uint index = 0) const override { /*KS_ENGINE_ASSERT(index < m_ColorTextures.size(), "Index is outside bounds (FBO)");*/ return m_ColorTextures[index]; }

	private:

		void SetTexture(bool depth_texture, GLenum format, uint width, uint height, uint samples);
		void AttachFramebufferTexture(uint texture_id, bool depth_texture, int samples, GLenum format, uint width, uint height, int index);

	private:

		uint m_ID = 0;
		FramebufferSettings m_FBOSettings;

		std::vector<FramebufferTextureSettings> m_ColorAttachmentSettings;
		std::vector<uint> m_ColorTextures;
		FramebufferTextureSettings m_DepthAttachmentSetting;
		uint m_DepthTexture = 0; // TODO: Change name to m_DepthTexture
	};

}
#endif //_OPENGL_FRAMEBUFFER_H_
