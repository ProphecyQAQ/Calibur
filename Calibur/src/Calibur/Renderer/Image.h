#pragma once

namespace Calibur
{
	enum class ImageFormat
	{
		Nono = 0,
		RGB,
		RGBA,
		RGB16F,
		RGBA16F,
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
