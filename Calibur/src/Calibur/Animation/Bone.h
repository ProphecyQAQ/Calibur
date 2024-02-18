#pragma once

#include "Calibur/Core/Base.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

namespace Calibur
{
	struct KeyPosition
	{
		glm::vec3 position;
		float timeStamp;
	};

	struct KeyRotation
	{
		glm::quat orientation;
		float timeStamp;
	};

	struct KeyScale
	{
		glm::vec3 scale;
		float timeStamp;
	};
	class Bone
	{
	public:
		Bone(const std::string& name, int ID, const aiNodeAnim* channel);

		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
		glm::mat4 GetLocalTransform() { return m_LocalTransform; }
		std::string GetBoneName() const { return m_Name; }
		int GetBoneID() { return m_ID; }

		void Update(float animationTime);

		int GetPositionIndex(float animationTime);	
		int GetRotationIndex(float animationTime);
		int GetScaleIndex(float animationTime);

		glm::mat4 InterpolatePosition(float animationTime);
		glm::mat4 InterpolateRotation(float animationTime);
		glm::mat4 InterpolateScaling(float animationTime);

	private:
		std::vector<KeyPosition> m_Positions;
		std::vector<KeyRotation> m_Rotations;
		std::vector<KeyScale> m_Scales;
		uint32_t m_NumPositions;
		uint32_t m_NumRotations;
		uint32_t m_NumScalings;

		glm::mat4 m_LocalTransform;
		std::string m_Name;
		int m_ID;
	};
}
