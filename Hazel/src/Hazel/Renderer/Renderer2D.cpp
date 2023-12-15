#include "hzpch.h"
#include "Hazel/Renderer/Renderer2D.h"

#include "Hazel/Renderer/VertexArray.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;
	};
	struct Renderer2DData
	{
		const uint32_t MaxQuads = 10000;
		const uint32_t MaxVertices = MaxQuads * 4;
		const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBase = nullptr;
		QuadVertex* QuadVertexPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture
	};

	static Renderer2DData s_Data;


	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNCTION();

		s_Data.QuadVertexArray = VertexArray::Create();

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));

		BufferLayout layout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float2, "a_TexCoord"},
			{ShaderDataType::Float, "a_TexIndex"},
			{ShaderDataType::Float, "a_TilingFactor"}
		};
		s_Data.QuadVertexBuffer->SetLayout(layout);
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBase = new QuadVertex[s_Data.MaxVertices];

		uint32_t offset = 0;
		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}
		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[]  quadIndices;

		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
		{
			samplers[i] = i;
		}

		//Init shader
		s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;
	}

	void Renderer2D::Shutdown()
	{
		HZ_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexPtr = s_Data.QuadVertexBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		HZ_PROFILE_FUNCTION();
		
		uint32_t dataSize = (uint8_t*)s_Data.QuadVertexPtr - (uint8_t*)s_Data.QuadVertexBase;
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBase, dataSize);
		Flush();
	}

	void Renderer2D::Flush()
	{
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
		{
			s_Data.TextureSlots[i]->Bind(i);
		}
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();
		
		const float textureIndex = 0.0f;
		const float tilingFactor = 1.0f;
			
		s_Data.QuadVertexPtr->Position = position;
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVertexPtr->TexIndex = textureIndex;
		s_Data.QuadVertexPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = { position.x + size.x, position.y, position.z };
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVertexPtr->TexIndex = textureIndex;
		s_Data.QuadVertexPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = { position.x + size.x, position.y + size.y, position.z };
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVertexPtr->TexIndex = textureIndex;
		s_Data.QuadVertexPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = { position.x, position.y + size.y, position.z };
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVertexPtr->TexIndex = textureIndex;
		s_Data.QuadVertexPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexPtr++;
		
		s_Data.QuadIndexCount += 6;
		/*s_Data.TextureShader->SetFloat("u_TilingFactor", 1.0f);
		s_Data.WhiteTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *  glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data.TextureShader->SetMat4("u_Transform", transform);

		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);*/
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, Ref<Texture2D>& texture, float tillingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tillingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, Ref<Texture2D>& texture, float tillingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		const glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		float textureIndex = 0;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		s_Data.QuadVertexPtr->Position = position;
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVertexPtr->TexIndex = textureIndex;
		s_Data.QuadVertexPtr->TilingFactor = tillingFactor;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = { position.x + size.x, position.y, position.z };
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVertexPtr->TexIndex = textureIndex;
		s_Data.QuadVertexPtr->TilingFactor = tillingFactor;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = { position.x + size.x, position.y + size.y, position.z };
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVertexPtr->TexIndex = textureIndex;
		s_Data.QuadVertexPtr->TilingFactor = tillingFactor;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = { position.x, position.y + size.y, position.z };
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVertexPtr->TexIndex = textureIndex;
		s_Data.QuadVertexPtr->TilingFactor = tillingFactor;
		s_Data.QuadVertexPtr++;

		s_Data.QuadIndexCount += 6;
	#if 0
		s_Data.TextureShader->SetFloat("u_TilingFactor", tillingFactor);
		s_Data.TextureShader->SetFloat4("u_Color", tintColor);
		texture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data.TextureShader->SetMat4("u_Transform", transform);

		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
	#endif
	}

	void Renderer2D::DrawRotateQuad(const glm::vec2& position, const glm::vec2& size, float rotate, glm::vec4& color)
	{
		DrawRotateQuad({ position.x, position.y, 0.0f }, size, rotate, color);
	}

	void Renderer2D::DrawRotateQuad(const glm::vec3& position, const glm::vec2& size, float rotate, glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.TextureShader->SetFloat("u_TilingFactor", 1.0f);
		s_Data.TextureShader->SetFloat4("u_Color", color);
		s_Data.WhiteTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotate, glm::vec3(0.f, 0.f, 1.f)) 
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data.TextureShader->SetMat4("u_Transform", transform);

		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
	}

	void Renderer2D::DrawRotateQuad(const glm::vec2& position, const glm::vec2& size, float rotate, Ref<Texture2D>& texture, float tillingFactor, const glm::vec4& tintColor)
	{
		DrawRotateQuad({ position.x, position.y, 0.0f }, size, rotate, texture, tillingFactor, tintColor);
	}

	void Renderer2D::DrawRotateQuad(const glm::vec3& position, const glm::vec2& size, float rotate, Ref<Texture2D>& texture, float tillingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.TextureShader->SetFloat("u_TilingFactor", tillingFactor);
		s_Data.TextureShader->SetFloat4("u_Color", tintColor);
		texture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotate, glm::vec3(0.f, 0.f, 1.f))
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data.TextureShader->SetMat4("u_Transform", transform);

		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
	}
}