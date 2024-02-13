#include "hzpch.h"
#include "Calibur/Renderer/Terrain.h"
#include "Calibur/Renderer/Renderer.h"

namespace Calibur
{
	Terrain::Terrain(Ref<Texture2D> heightMap)
		: m_HeightMap(heightMap)
	{
		m_Shader = Renderer::GetShaderLibrary()->Get("Terrain");

		float width = m_HeightMap->GetWidth(), height = m_HeightMap->GetHeight();

		std::vector<float> vertices;
		uint32_t res = 20;
		unsigned rez = 20;
		for(unsigned i = 0; i < rez; i++)
		{
			for(unsigned j = 0; j < rez; j++)
			{
				vertices.push_back(-width/2.0f + width*i/(float)rez); // v.x
				vertices.push_back(0.0f); // v.y
				vertices.push_back(-height/2.0f + height*j/(float)rez); // v.z
				vertices.push_back(i / (float)rez); // u
				vertices.push_back(j / (float)rez); // v

				vertices.push_back(-width/2.0f + width*(i+1)/(float)rez); // v.x
				vertices.push_back(0.0f); // v.y
				vertices.push_back(-height/2.0f + height*j/(float)rez); // v.z
				vertices.push_back((i+1) / (float)rez); // u
				vertices.push_back(j / (float)rez); // v

				vertices.push_back(-width/2.0f + width*i/(float)rez); // v.x
				vertices.push_back(0.0f); // v.y
				vertices.push_back(-height/2.0f + height*(j+1)/(float)rez); // v.z
				vertices.push_back(i / (float)rez); // u
				vertices.push_back((j+1) / (float)rez); // v

				vertices.push_back(-width/2.0f + width*(i+1)/(float)rez); // v.x
				vertices.push_back(0.0f); // v.y
				vertices.push_back(-height/2.0f + height*(j+1)/(float)rez); // v.z
				vertices.push_back((i+1) / (float)rez); // u
				vertices.push_back((j+1) / (float)rez); // v
			}
		}
		
		m_VertexArray = VertexArray::Create();
		m_VertexBuffer = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(float));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		};
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_Size = 4 * rez * rez;
	}

	void Terrain::Render()
	{
		m_Shader->Bind();
		m_HeightMap->Bind(13);
		RenderCommand::DrawPatches(m_VertexArray, m_Size);
	}
}
