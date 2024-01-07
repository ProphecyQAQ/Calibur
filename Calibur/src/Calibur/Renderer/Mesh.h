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

		std::string MeshName;
	};

	class Mesh
	{
	public:
		Mesh(std::string filepath);
		~Mesh() = default;

		// TODO: Remove
		void LoadMaterials(const aiScene* scene);

		Ref<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }
		Ref<VertexArray> GetVertexArray() { return m_VertexArray; }

		std::vector<SubMesh>& GetSubMeshes() { return m_SubMeshes; }
		std::vector<Ref<Material>>& GetMaterials() { return m_Materials; }

	private:
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
		Ref<VertexArray> m_VertexArray;

		std::vector<SubMesh> m_SubMeshes;

		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;

		std::vector<Ref<Material>> m_Materials;

		std::string m_FilePath;
	};
}
