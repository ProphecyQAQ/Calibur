#include "hzpch.h"
#include "Calibur/Renderer/Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Calibur
{
	static const uint32_t s_MeshImportFlags =
		aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
		aiProcess_Triangulate |             // Make sure we're triangles
		aiProcess_SortByPType |             // Split meshes by primitive type
		aiProcess_GenNormals |              // Make sure we have legit normals
		aiProcess_GenUVCoords |             // Convert UVs if required 
		//aiProcess_OptimizeGraph |          
		//aiProcess_OptimizeMeshes |          // Batch draws where possible
		//aiProcess_JoinIdenticalVertices |   // Join identical vertices/ optimize indexing
		aiProcess_ValidateDataStructure;    // Validation


	Mesh::Mesh(std::string filepath)
		: m_FilePath(filepath)
	{	
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(filepath,  s_MeshImportFlags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			HZ_CORE_ERROR("Assimp import error::{0}", importer.GetErrorString());
			HZ_CORE_ASSERT(false);
		}

		if (scene->HasMeshes())
		{
			uint32_t vertexCount = 0;
			uint32_t indexCount  = 0;

			m_SubMeshes.resize(scene->mNumMeshes);
			for (size_t idx = 0; idx < scene->mNumMeshes; idx++)
			{
				aiMesh* mesh = scene->mMeshes[idx];

				m_SubMeshes[idx].BaseVertex = vertexCount;
				m_SubMeshes[idx].BaseIndex = indexCount;
				m_SubMeshes[idx].MaterialIndex = mesh->mMaterialIndex;
				m_SubMeshes[idx].VertexCount = mesh->mNumVertices;
				m_SubMeshes[idx].IndexCount = mesh->mNumFaces * 3;

				vertexCount += mesh->mNumVertices;
				indexCount += mesh->mNumFaces * 3;

				// Vertices
				for (size_t i = 0; i < mesh->mNumVertices; i++)
				{
					Vertex vertex;

					vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
					vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

					if (mesh->HasTangentsAndBitangents())
					{
						vertex.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
						vertex.bitangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
					}

					if (mesh->HasTextureCoords(0))
					{
						vertex.texCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
					}

					m_Vertices.push_back(vertex);
				}

				// Indecis
				for (size_t i = 0; i < mesh->mNumFaces; i++)
				{
					aiFace& face = mesh->mFaces[i];
					HZ_CORE_ASSERT(face.mNumIndices == 3, "Calibur only supports triangles for now!");
					Index index = { face.mIndices[0], face.mIndices[1], face.mIndices[2] };
					m_Indices.push_back(index);
				}
			}
		}
			
		m_VertexBuffer = VertexBuffer::Create((float*)m_Vertices.data(), (uint32_t)(m_Vertices.size() * sizeof(Vertex)));
		m_IndexBuffer = IndexBuffer::Create((uint32_t*)m_Indices.data(), (uint32_t)(m_Indices.size() * sizeof(Index)));

		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoords" },
			{ ShaderDataType::Float3, "a_Tangent" },
			{ ShaderDataType::Float3, "a_Bitangent" }
		};
		m_VertexBuffer->SetLayout(layout);

		m_VertexArray = VertexArray::Create();
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}
}
