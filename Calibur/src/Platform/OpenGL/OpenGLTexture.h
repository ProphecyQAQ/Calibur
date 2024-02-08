#pragma once

#include "Calibur/Renderer/Texture.h"

#include <glad/glad.h>

namespace Calibur
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& specification);
		OpenGLTexture2D(const TextureSpecification& specification, const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual void SetData(void* data, uint32_t size) override;
		virtual void CopyDataFromAnotherTexture(uint32_t texID) override;
		virtual void GenerateMipmap() override;
		
		virtual void Bind(uint32_t slot = 0) const override;
		virtual bool IsLoaded() const override { return m_IsLoaded; }

		virtual bool operator==(const Texture& other) const override 
		{
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		};
	private:
		std::string m_Path;
		bool m_IsLoaded = false;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat, m_DataType, m_Wrap, m_Min_Filter, m_Mag_Filter;
	};

	class OpenGLTexture2DArray : public Texture2DArray
	{
	public:
		OpenGLTexture2DArray(const TextureSpecification& specification);

		virtual ~OpenGLTexture2DArray() = default;

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual void SetData(void* data, uint32_t size) override {}
		virtual void CopyDataFromAnotherTexture(uint32_t texID) {};
		virtual void GenerateMipmap() override {}
		
		virtual void Bind(uint32_t slot = 0) const override;
		virtual bool IsLoaded() const override { return m_IsLoaded; }

		virtual bool operator==(const Texture& other) const override 
		{
			return m_RendererID == ((OpenGLTexture2DArray&)other).m_RendererID;
		};
	private:
		std::string m_Path;
		bool m_IsLoaded = false;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		uint32_t m_ArraySize;
		GLenum m_InternalFormat, m_DataFormat, m_DataType, m_Wrap, m_Min_Filter, m_Mag_Filter;
	};

	class OpenGLTextureCube : public TextureCube
	{
	public:
		OpenGLTextureCube(const TextureSpecification& specification);
		OpenGLTextureCube(const TextureSpecification& specification, const std::string& directoryPath);
		virtual ~OpenGLTextureCube();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }
		virtual void SetData(void* data, uint32_t size) override {//todo
		};
		virtual void CopyDataFromAnotherTexture(uint32_t texID) {};
		virtual void GenerateMipmap() override;

		virtual void Bind(uint32_t slot = 0) const override;
		virtual bool IsLoaded() const override { return m_IsLoaded; }

		virtual bool operator==(const Texture& other) const override 
		{
			return m_RendererID == ((OpenGLTextureCube&)other).m_RendererID;
		};

	private:
		std::string m_DirectoryPath;
		bool m_IsLoaded = false;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat, m_DataType, m_Wrap, m_Min_Filter, m_Mag_Filter;
	};
}
