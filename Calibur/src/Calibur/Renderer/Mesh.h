#pragma once

#include "Calibur/Renderer/Buffer.h"
#include "Calibur/Renderer/VertexArray.h"
#include "Calibur/Renderer/Material.h"

#include <glm/glm.hpp>

#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Calibur 
{
	struct Vertex 
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;

		glm::vec3 tangent;
		glm::vec3 bitangent;
	};

	struct Index
	{
		uint32_t id1, id2, id3;
	};

	class SubMesh
	{
	public:
		uint32_t BaseVertex;
		uint32_t BaseIndex;
		uint32_t MaterialIndex;
		uint32_t IndexCount;
		uint32_t VertexCount;

		glm::mat4 Transform{ 1.0f };
		glm::mat4 LocalTransform{ 1.0f };

		std::string MeshName, NodeName;
	};

	class MeshNode
	{
	public:
		uint32_t Parent = 0xffffffff;
		std::vector<uint32_t> Children;
		std::vector<uint32_t> SubMeshes;
		
		std::string Name;
		glm::mat4 LocalTransform;

		inline bool IsRoot() const { return Parent == 0xffffffff; }
	};

	class Mesh
	{
	public:
		Mesh(std::string filepath, bool isVerticalFlip);
		~Mesh() = default;

		// TODO: Remove
		void LoadMaterials(const aiScene* scene);

		Ref<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }
		Ref<VertexArray> GetVertexArray() { return m_VertexArray; }
		bool IsLoaded() const { return m_IsLoaded; }

		std::vector<SubMesh>& GetSubMeshes() { return m_SubMeshes; }
		std::vector<Ref<Material>>& GetMaterials() { return m_Materials; }
		std::string& GetFilePath() { return m_FilePath; }
	private:
		void TraverseNodes(aiNode* aNode, uint32_t nodeIndex, const glm::mat4& parentTransform = glm::mat4(1.0f));

	private:
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
		Ref<VertexArray> m_VertexArray;

		std::vector<SubMesh> m_SubMeshes;
		std::vector<MeshNode> m_MeshNodes;

		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;

		std::vector<Ref<Material>> m_Materials;
		bool m_IsVerticalFlip = false;
		bool m_IsLoaded = false;

		std::string m_FilePath;
	};
}
