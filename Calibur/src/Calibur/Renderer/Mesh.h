#pragma once

#include "Calibur/Renderer/Buffer.h"
#include "Calibur/Renderer/VertexArray.h"
#include "Calibur/Renderer/Material.h"

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Calibur 
{
	#define MAX_BONE_INFLUENCE 4
	#define MAX_BONE_WEIGHTS 4
	struct Vertex 
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;

		glm::vec3 tangent;
		glm::vec3 bitangent;

		//bone indexes which will influence this vertex
		int m_BoneIDs[MAX_BONE_INFLUENCE] = { -1, -1, -1, -1 };
		//weights from each bone
		float m_Weights[MAX_BONE_INFLUENCE] = { 0.0, 0.0, 0.0, 0.0 };
	};

	struct BoneInfo
	{
		uint32_t BoneIndex;
		glm::mat4 OffsetMatrix; // convert vertex form model space to bone space
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
		Mesh(std::string filepath, const std::string& shaderName, bool isVerticalFlip);
		~Mesh() = default;

		// TODO: Remove
		void LoadMaterials(const aiScene* scene);
		void Mesh::SetVertexBoneData(Vertex& vertex, int boneID, float weight);

		Ref<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }
		Ref<VertexArray> GetVertexArray() { return m_VertexArray; }
		bool IsLoaded() const { return m_IsLoaded; }

		std::vector<SubMesh>& GetSubMeshes() { return m_SubMeshes; }
		std::vector<MeshNode>& GetMeshNodes() { return m_MeshNodes; }
		std::vector<Ref<Material>>& GetMaterials() { return m_Materials; }
		std::string& GetFilePath() { return m_FilePath; }
		std::string& GetShaderName() { return m_ShaderName; }

		auto& GetBoneInfoMap() { return m_BoneInfoMap; }
		uint32_t& GetBoneCount() { return m_BoneCount; } 
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

		std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
		uint32_t m_BoneCount = 0;
		
		std::string m_ShaderName;
		std::vector<Ref<Material>> m_Materials;
		bool m_IsVerticalFlip = false;
		bool m_IsLoaded = false;

		std::string m_FilePath;
	};
}
