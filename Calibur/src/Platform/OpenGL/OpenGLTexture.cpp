#include "hzpch.h"
#include "Platform/OpenGL/OpenGLTexture.h"

#include <stb_image.h>

namespace Calibur
{
	namespace Utils
	{
		static GLenum OpenGLTextureFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB:
				case ImageFormat::RGB16F:
					return GL_RGB;
				case ImageFormat::RGBA:	
					return GL_RGBA;
				case ImageFormat::RG16F:
					return GL_RG;
			}

			HZ_CORE_ASSERT(false, "Unknown TextureFormat!");
			return 0;
		}

		static GLenum OpenGLTextureInternalFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB:		
					return GL_RGB8;
				case ImageFormat::RGBA:		
					return GL_RGBA8;
				case ImageFormat::RGB16F:	
					return GL_RGB16F;
				case ImageFormat::RG16F:
					return GL_RG16F;
			}

			HZ_CORE_ASSERT(false, "Unknown TextureFormat!");
			return 0;
		}

		static GLenum OpenGLTextureDataType(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB:		
				case ImageFormat::RGBA:		
					return GL_UNSIGNED_BYTE;
				case ImageFormat::RGB16F:
				case ImageFormat::RG16F:
					return GL_FLOAT;
			}

			HZ_CORE_ASSERT(false, "Unknown TextureFormat!");
			return 0;
		}

		static GLenum OpenGLTextureWrap(TextureWrap wrap)
		{
			switch (wrap)
			{
				case TextureWrap::None:		
					return GL_NONE;
				case TextureWrap::Clamp:	
					return GL_CLAMP_TO_EDGE;
				case TextureWrap::Repeat:	
					return GL_REPEAT;
			}

			HZ_CORE_ASSERT(false, "Unknown TextureWrap!");
			return 0;
		}

		static GLenum OpenGLTextureFilter(TextureFilter filter)
		{
			switch (filter)
			{
				case TextureFilter::None:		
					return GL_NONE;
				case TextureFilter::Linear:	
					return GL_LINEAR;
				case TextureFilter::Nearest:	
					return GL_NEAREST;
			}

			HZ_CORE_ASSERT(false, "Unknown TextureFilter!");
			return 0;
		}
	}


	////////////////////////////////
	////////Texture 2D////////////
	////////////////////////////////

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification)
		: m_Width(specification.Width), m_Height(specification.Height)
	{
		HZ_PROFILE_FUNCTION();

		m_InternalFormat = Utils::OpenGLTextureInternalFormat(specification.Format);
		m_DataFormat = Utils::OpenGLTextureFormat(specification.Format);
		m_DataType = Utils::OpenGLTextureDataType(specification.Format);
		m_Wrap = Utils::OpenGLTextureWrap(specification.Wrap);
		m_Filter = Utils::OpenGLTextureFilter(specification.Filter);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		if (specification.isGenerateMipMap) {
			glGenerateTextureMipmap(m_RendererID);
			m_Filter = GL_LINEAR_MIPMAP_LINEAR;
		}

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, m_Filter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, m_Filter);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, m_Wrap);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, m_Wrap);
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification, const std::string& path)
		: m_Path(path)
	{	
		HZ_PROFILE_FUNCTION();

		int width, height, channels, type = 0;
		stbi_set_flip_vertically_on_load(specification.isVerticalFlip);
		void* data = nullptr;
		{
			HZ_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");

			if (stbi_is_hdr(path.c_str()))
				data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
			else
				data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		if (data)
		{
			m_IsLoaded = true;

			m_Width = width;
			m_Height = height;

			GLenum internalFormat = 0, dataFormat = 0;

			if (stbi_is_hdr(path.c_str()))
			{
				if (channels == 3)
				{
					internalFormat = GL_RGB16F;
					dataFormat = GL_RGB;
				}
				else if (channels == 2)
				{
					internalFormat = GL_RG16F;
					dataFormat = GL_RG;
				}
				type = GL_FLOAT;
			}
			else
			{
				if (channels == 4)
				{
					internalFormat = GL_RGBA8;
					dataFormat = GL_RGBA;
				}
				else if (channels == 3)
				{
					internalFormat = GL_RGB8;
					dataFormat = GL_RGB;
				}
				type = GL_UNSIGNED_BYTE;
			}

			m_InternalFormat = internalFormat;
			m_DataFormat = dataFormat;
			m_DataType = type;
			m_Wrap = Utils::OpenGLTextureWrap(specification.Wrap);
			m_Filter = Utils::OpenGLTextureFilter(specification.Filter);

			HZ_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, m_Filter);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, m_Filter);

			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, m_Wrap);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, m_Wrap);

			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, type, data);
			
			stbi_image_free(data);
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		HZ_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		HZ_PROFILE_FUNCTION();
		
		uint32_t bpc = m_DataFormat == GL_RGBA ? 4 : 3;
		HZ_CORE_ASSERT(size == m_Height * m_Width * bpc, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::GenerateMipmap()
	{
		glGenerateTextureMipmap(m_RendererID);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		HZ_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}

	////////////////////////////////
	////////Texture Cube////////////
	////////////////////////////////
	
	static const char* direction[] = { "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg" };

	OpenGLTextureCube::OpenGLTextureCube(const TextureSpecification& specification)
		: m_Width(specification.Width), m_Height(specification.Height)
	{
		m_InternalFormat = Utils::OpenGLTextureInternalFormat(specification.Format);
		m_DataFormat = Utils::OpenGLTextureFormat(specification.Format);
		m_DataType = Utils::OpenGLTextureDataType(specification.Format);
		m_Wrap = Utils::OpenGLTextureWrap(specification.Wrap);
		m_Mag_Filter = Utils::OpenGLTextureFilter(specification.Filter);
		if (specification.isGenerateMipMap) {
			m_Min_Filter = GL_LINEAR_MIPMAP_LINEAR;
		}
		else m_Min_Filter = m_Mag_Filter;

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, m_DataType, nullptr);
		}

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, m_Wrap);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, m_Wrap);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, m_Wrap);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, m_Min_Filter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, m_Mag_Filter);

		if (specification.isGenerateMipMap) {
			glGenerateTextureMipmap(m_RendererID);
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	OpenGLTextureCube::OpenGLTextureCube(const TextureSpecification& specification, const std::string& directoryPath)
	{
		HZ_PROFILE_FUNCTION();

		for (size_t i = 0; i < 6; i++)
		{
			std::string path = directoryPath + "/" + direction[i];

			int width, height, channels;
			stbi_set_flip_vertically_on_load(specification.isVerticalFlip);
			stbi_uc* data = nullptr;

			data = stbi_load(path.c_str(), &width, &height, &channels, 0);

			if (data == nullptr) continue;

			GLenum internalFormat = 0, dataFormat = 0;
			if (channels == 4)
			{
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
			}
			else if (channels == 3)
			{
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
			}

			m_InternalFormat = internalFormat;
			m_DataFormat = dataFormat;
			m_DataType = GL_UNSIGNED_BYTE;
			m_Wrap = Utils::OpenGLTextureWrap(specification.Wrap);
			m_Min_Filter = Utils::OpenGLTextureFilter(specification.Filter);
			m_Mag_Filter = m_Min_Filter;
			
			if (i == 0) {
				glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
				glTextureStorage2D(m_RendererID, 1, internalFormat, width, height);
			}
			
			glTextureSubImage3D(m_RendererID, 0, 0, 0, i, width, height, 1, dataFormat, m_DataType, data);

			stbi_image_free(data);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, m_Min_Filter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, m_Mag_Filter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, m_Wrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, m_Wrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, m_Wrap);
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTextureCube::GenerateMipmap()
	{
		glGenerateTextureMipmap(m_RendererID);
	}

	void OpenGLTextureCube::Bind(uint32_t slot) const
	{
		HZ_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}
}
