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
	};

	enum class TextureWrap
	{
		None = 0,
		Clamp,
		Repeat,
	};

	enum class TextureFilter
	{
		None = 0,
		Linear,
		Nearest,

	};
}
