#pragma once

#include <glm/glm.hpp>

namespace Calibur::Math {

	bool DecomposeTransform(glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);
}
