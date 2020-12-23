#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_


namespace Kaimos {

	struct FramebufferSettings
	{
		uint width, height, samples = 1;
		bool swapchain_target = false;
	};

	class Framebuffer
	{
	public:

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual const FramebufferSettings& GetFBOSettings() const = 0;
		virtual const uint GetFBOTextureID() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSettings& settings);
	};

}
#endif //_FRAMEBUFFER_H_