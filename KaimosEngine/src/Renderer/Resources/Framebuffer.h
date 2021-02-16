#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_


namespace Kaimos {

	enum class TEXTURE_FORMAT
	{
		NONE = 0,
		RGBA8,				// Color
		DEPTH24STENCIL8,	// Depth & Stencil

		// Defaults
		DEPTH = DEPTH24STENCIL8
	};

	struct FramebufferTextureSettings // Format, Wrap, Filtering...
	{
		FramebufferTextureSettings() = default;
		FramebufferTextureSettings(TEXTURE_FORMAT format)
			: TextureFormat(format) {}

		TEXTURE_FORMAT TextureFormat = TEXTURE_FORMAT::NONE;
		// TODO: Filtering & Wrapping
	};

	struct FramebufferAttachmentSettings
	{
		FramebufferAttachmentSettings() = default;
		FramebufferAttachmentSettings(std::initializer_list<FramebufferTextureSettings> texture_attachments)
			: TextureAttachments(texture_attachments) {}

		std::vector<FramebufferTextureSettings> TextureAttachments;
	};

	struct FramebufferSettings
	{
		uint Width = 0, Height = 0;
		FramebufferAttachmentSettings FBOAttachments;
		uint Samples = 1;
		bool SwapChain_Target = false;
	};

	class Framebuffer
	{
	public:

		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual const FramebufferSettings& GetFBOSettings() const = 0;
		virtual const uint GetFBOTextureID(uint index = 0) const = 0;
		virtual void Resize(uint width, uint height) = 0;

		static Ref<Framebuffer> Create(const FramebufferSettings& settings);
	};

}
#endif //_FRAMEBUFFER_H_
