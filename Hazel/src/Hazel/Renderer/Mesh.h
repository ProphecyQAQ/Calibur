#pragma once

#include <glm/glm.hpp>

namespace Hazel 
{
	struct Vertex 
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		glm::vec2 texCoords;
	};

	class Mesh
	{
	public:
		Mesh() = default;
		~Mesh() = default;

	private:
	};
}
