#include "hzpch.h"
#include "Calibur/Renderer/Mesh.h"
#include "Calibur/Renderer/Renderer.h"

namespace Calibur
{
	namespace Utils
	{
		glm::mat4 ConvertAiMat4ToGlm(const aiMatrix4x4& aiMat) {
			// glm::mat4 is column-major, while aiMatrix4x4 is row-major
			return glm::mat4(
				aiMat.a1, aiMat.a2, aiMat.a3, aiMat.a4,
				aiMat.b1, aiMat.b2, aiMat.b3, aiMat.b4,
				aiMat.c1, aiMat.c2, aiMat.c3, aiMat.c4,
				aiMat.d1, aiMat.d2, aiMat.d3, aiMat.d4
			);
		}
	}

	static const uint32_t s_MeshImportFlags =
		aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
		aiProcess_Triangulate |             // Make sure we're triangles
		aiProcess_GenNormals |              // Make sure we have legit normals
		//aiProcess_SortByPType |             // Split meshes by primitive type
		aiProcess_GenUVCoords |             // Convert UVs if required 
		//aiProcess_OptimizeGraph |          
		aiProcess_OptimizeMeshes |          // Batch draws where possible
		aiProcess_JoinIdenticalVertices |   // Join identical vertices/ optimize indexing
		aiProcess_GlobalScale |             // e.g. convert cm to m for fbx import (and other formats where cm is native)
		aiProcess_ValidateDataStructure |    // Validation
		aiProcess_PreTransformVertices;


	Mesh::Mesh(std::string filepath, bool isVerticalFlip)
		: m_FilePath(filepath), m_IsVerticalFlip(isVerticalFlip)
	{	
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(filepath,  s_MeshImportFlags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			HZ_CORE_ERROR("Assimp import error::{0}", importer.GetErrorString());
			HZ_CORE_ASSERT(false);
		}
		
		m_IsLoaded = true;

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
				m_SubMeshes[idx].MeshName = mesh->mName.C_Str();

				vertexCount += mesh->mNumVertices;
				indexCount  += mesh->mNumFaces * 3;

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

		MeshNode& root = m_MeshNodes.emplace_back();
		TraverseNodes(scene->mRootNode, 0);

		LoadMaterials(scene);
			
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
	

	void Mesh::LoadMaterials(const aiScene* scene)
	{
		if (scene->HasMaterials())
		{
			m_Materials.resize(scene->mNumMaterials);

			for (size_t i = 0; i < scene->mNumMaterials; i++)
			{
				auto aiMaterial = scene->mMaterials[i];
				auto aiMateriaName = aiMaterial->GetName();

				Ref<Material> material = Material::Create(Renderer::GetShaderLibrary()->Get("Pbr_withMotion"), aiMateriaName.C_Str());
				m_Materials[i] = material;
				
				///////////// Material Properties //////////////
				aiColor3D aiColor;
				if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor) == AI_SUCCESS)
				{
					material->GetMaterialUniforms().Albedo = {aiColor.r, aiColor.g, aiColor.b, 1.0f};
				}

				if (aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, aiColor) == AI_SUCCESS)
				{
					material->GetMaterialUniforms().Emission = aiColor.r;
				}
				
				float metallic, roughness;
				if (aiMaterial->Get(AI_MATKEY_REFLECTIVITY, metallic) == AI_SUCCESS)
				{
					material->GetMaterialUniforms().Metallic = metallic;
				}
				if (aiMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS)
				{
					material->GetMaterialUniforms().Roughness = roughness;
				}
				///////////////////////////////////////////////

				///////////// Material Textures  //////////////
				// Now one material only can load one texture for each textures.

				TextureSpecification spec;
				spec.isVerticalFlip = m_IsVerticalFlip;
				spec.isGenerateMipMap = true;
				// Diffuse
				if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
				{
					aiString path;
					aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path);

					size_t lastSlash = m_FilePath.find_last_of('/');
					std::string texturePath = m_FilePath.substr(0, lastSlash + 1) + path.C_Str();
					material->SetDiffuseMap(Texture2D::Create(spec, texturePath));
				}
				else material->SetDiffuseMap(Renderer::GetWhiteTexture());
				// Normal
				if (aiMaterial->GetTextureCount(aiTextureType_NORMALS) > 0)
				{
					aiString path;
					aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &path);

					size_t lastSlash = m_FilePath.find_last_of('/');
					std::string texturePath = m_FilePath.substr(0, lastSlash + 1) + path.C_Str();
					material->SetNormalMap(Texture2D::Create(spec, texturePath));

					material->GetMaterialUniforms().useNormalMap = 1;
				}

				// Roughness
				if (aiMaterial->GetTextureCount(aiTextureType_SHININESS) > 0)
				{
					aiString path;
					aiMaterial->GetTexture(aiTextureType_SHININESS, 0, &path);

					size_t lastSlash = m_FilePath.find_last_of('/');
					std::string texturePath = m_FilePath.substr(0, lastSlash + 1) + path.C_Str();
					material->SetRoughnessMap(Texture2D::Create(spec, texturePath));
				}
				else material->SetRoughnessMap(Renderer::GetWhiteTexture());
				// Specular// Metallic
				if (aiMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
				{
					aiString path;
					aiMaterial->GetTexture(aiTextureType_SPECULAR, 0, &path);

					size_t lastSlash = m_FilePath.find_last_of('/');
					std::string texturePath = m_FilePath.substr(0, lastSlash + 1) + path.C_Str();
					material->SetSpecMap(Texture2D::Create(spec, texturePath));
				}
				else material->SetSpecMap(Renderer::GetWhiteTexture());

				///////////////////////////////////////////////
			}
		}
	}

	void Mesh::TraverseNodes(aiNode* aNode, uint32_t nodeIndex, const glm::mat4& parentTransform)
	{
		MeshNode& node = m_MeshNodes[nodeIndex];
		node.Name = aNode->mName.C_Str();
		node.LocalTransform = Utils::ConvertAiMat4ToGlm(aNode->mTransformation);

		glm::mat4 transform = parentTransform * node.LocalTransform;
		for (size_t i = 0; i < aNode->mNumMeshes; i++)
		{
			uint32_t submeshIndex = aNode->mMeshes[i];
			auto& submesh = m_SubMeshes[submeshIndex];
			submesh.NodeName = node.Name;
			submesh.Transform = transform;
			submesh.LocalTransform = node.LocalTransform;

			node.SubMeshes.push_back(submeshIndex);
		}

		uint32_t parentNodeIndex = (uint32_t)m_MeshNodes.size() - 1;
		node.Children.resize(aNode->mNumChildren);
		for (size_t i = 0; i < aNode->mNumChildren; i++)
		{
			MeshNode& child = m_MeshNodes.emplace_back();
			size_t childIndex = m_MeshNodes.size() - 1;
			child.Parent = parentNodeIndex;
			m_MeshNodes[nodeIndex].Children[i] = childIndex;
			TraverseNodes(aNode->mChildren[i], (uint32_t)childIndex, transform);
		}
	}
}
