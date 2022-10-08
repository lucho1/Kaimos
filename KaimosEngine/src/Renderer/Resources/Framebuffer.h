#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

namespace Kaimos {

	enum class TEXTURE_TARGET
	{
		NONE = 0,
		TEXTURE_2D,
		TEXTURE_CUBEMAP
	};

	enum class TEXTURE_FORMAT
	{
		NONE = 0,
		RGBA8,				// Color
		RED_INTEGER,		// Red Color Channel
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

		// --- Public Class Methods ---
		virtual ~Framebuffer() = default;

		// --- Public FBO Methods ---
		virtual void Bind(uint width = 0, uint height = 0) = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint width, uint height, bool generate_depth_renderbuffer = false) = 0;
		virtual void ClearFBOTexture(uint index, int value) = 0;

		virtual void AttachColorTexture(TEXTURE_TARGET target, uint target_index, uint texture_id, uint mip_level = 0) = 0;
		virtual void CreateAndAttachRedTexture(uint target_index, uint width, uint height) = 0;

		virtual void ResizeAndBindRenderBuffer(uint width, uint height) = 0;
		
		static Ref<Framebuffer> Create(const FramebufferSettings& settings, bool generate_depth_renderbuffer = false);
		static Ref<Framebuffer> CreateEmptyAndBind(uint width, uint height, bool generate_depth_renderbuffer = false);
		
	public:

		// --- Getters ---
		virtual int GetPixelFromFBO(uint index, int x, int y) = 0;
		virtual uint GetFBOTextureID(uint index = 0)		const = 0;
		virtual const FramebufferSettings& GetFBOSettings()	const = 0;
	};
}

#endif //_FRAMEBUFFER_H_
