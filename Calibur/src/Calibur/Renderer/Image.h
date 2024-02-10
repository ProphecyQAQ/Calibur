#pragma once

namespace Calibur
{
	enum class ImageFormat
	{
		Nono = 0,
		RGB,
		RGB16F,

		RGBA,
		RGBA16F,
		
		RG16F,

		DEPTH24STENCIL8,
		DEPTH32FSTENCIL8,
		DEPTH32F,
	};

	enum class TextureWrap
	{
		None = 0,
		Repeat,
		ClampToEdge,
		ClampToBorder,
	};

	enum class TextureFilter
	{
		None = 0,
		Linear,
		Nearest,
	};
}
