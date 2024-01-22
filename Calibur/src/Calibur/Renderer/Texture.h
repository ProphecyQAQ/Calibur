#pragma once

#include "Calibur/Core/Base.h"
#include "Calibur/Renderer/Image.h"

namespace Calibur
{
	struct TextureSpecification
	{
		ImageFormat Format = ImageFormat::RGBA;
		TextureWrap Wrap = TextureWrap::Repeat;
		TextureFilter Filter = TextureFilter::Linear;

		uint32_t Width = 1;
		uint32_t Height = 1;
		uint32_t Samples = 1;
		
		bool isGenerateMipMap = false;
		bool isVerticalFlip = false;
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void GenerateMipmap() = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool IsLoaded() const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const TextureSpecification& specification);
		static Ref<Texture2D> Create(const TextureSpecification& specification, const std::string& path);
	};

	class TextureCube : public Texture
	{
	public:
		static Ref<TextureCube> Create(const TextureSpecification& specification);
		static Ref<TextureCube> Create(const TextureSpecification& specification, const std::string& directoryPath);
	};
}
