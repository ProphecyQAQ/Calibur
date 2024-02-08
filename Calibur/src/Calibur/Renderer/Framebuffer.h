#pragma once

#include "Calibur/Core/Base.h"

namespace Calibur
{
	enum class FramebufferTextureFormat
	{
		None = 0,
		
		//Color
		RGBA8,
		RGB16F,
		RG16F,
		RED_INTEGER,

		//Depth/stencil
		DEPTH24STENCIL8,
		DEPTH32FSTENCIL8,
		
		//Defaults
		Depth = DEPTH24STENCIL8,
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			:TextureFormat(format) {}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;
		FramebufferAttachmentSpecification Attachments;
		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int i, int j) = 0;

		virtual void SetRenderTargetToTextureCube(uint32_t renderID, uint32_t index, uint32_t mipLevel = 0) = 0;
		virtual void SetRenderTargetToTexture2D(uint32_t renderID, uint32_t mipLevel = 0) = 0;
		virtual void SetDepthAttachment(uint32_t renderID) = 0;
		
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;
		virtual void ClearAttachment(uint32_t attachmentIndex, glm::vec2& value) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual uint32_t GetDepthAttachmentRendererID() const = 0;

		virtual const FramebufferSpecification& GetSpecificaition() const = 0;
		virtual float GetWidth() = 0;
		virtual float GetHeight() = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}
