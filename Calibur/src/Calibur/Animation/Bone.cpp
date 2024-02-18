#include "hzpch.h"

#include "Calibur/Animation/Bone.h"

namespace Calibur
{
	Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
		: m_ID(ID), m_Name(name), m_LocalTransform(glm::mat4(1.0f))
	{
		m_NumPositions = channel->mNumPositionKeys;

		for (size_t id = 0; id < m_NumPositions; id++)
		{
			aiVector3D aiPos = channel->mPositionKeys[id].mValue;
			float timeStamp = channel->mPositionKeys[id].mTime;

			KeyPosition key;
			key.position = { aiPos.x, aiPos.y, aiPos.z };
			key.timeStamp = timeStamp;
			m_Positions.push_back(key);
		}

		m_NumRotations = channel->mNumRotationKeys;

		for (size_t id = 0; id < m_NumRotations; id++)
		{
			aiQuaternion aiRot = channel->mRotationKeys[id].mValue;
			float timeStamp = channel->mRotationKeys[id].mTime;

			KeyRotation key;
			key.orientation = { aiRot.w, aiRot.x, aiRot.y, aiRot.z };
			key.timeStamp = timeStamp;
			m_Rotations.push_back(key);
		}

		m_NumScalings = channel->mNumScalingKeys;

		for (size_t id = 0; id < m_NumScalings; id++)
		{
			aiVector3D aiScale = channel->mScalingKeys[id].mValue;
			float timeStamp = channel->mScalingKeys[id].mTime;

			KeyScale key;
			key.scale = { aiScale.x, aiScale.y, aiScale.z };
			key.timeStamp = timeStamp;
			m_Scales.push_back(key);
		}
	}

	float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
	{
        float scaleFactor = 0.0f;
        float midWayLength = animationTime - lastTimeStamp;
        float framesDiff = nextTimeStamp - lastTimeStamp;
        scaleFactor = midWayLength / framesDiff;
		HZ_CORE_ASSERT(scaleFactor <= 1.0 && scaleFactor >= 0.0, "Interpolate factor error");
        return scaleFactor;
	}

	void Bone::Update(float animationTime)
	{
		glm::mat4 translation = InterpolatePosition(animationTime);
        glm::mat4 rotation = InterpolateRotation(animationTime);
        glm::mat4 scale = InterpolateScaling(animationTime);

        m_LocalTransform = translation * rotation * scale;
	}

	int Bone::GetPositionIndex(float animationTime)
    {
        for (int index = 0; index < m_NumPositions - 1; ++index)
        {
            if (animationTime < m_Positions[index + 1].timeStamp)
                return index;
        }
    }

    int Bone::GetRotationIndex(float animationTime)
    {
        for (int index = 0; index < m_NumRotations - 1; ++index)
        {
            if (animationTime < m_Rotations[index + 1].timeStamp)
                return index;
        }
    }

    int Bone::GetScaleIndex(float animationTime)
    {
        for (int index = 0; index < m_NumScalings - 1; ++index)
        {
            if (animationTime < m_Scales[index + 1].timeStamp)
                return index;
        }
    }

	// private
	glm::mat4 Bone::InterpolatePosition(float animationTime)
	{
		if (m_NumPositions == 1)
			return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

		int p0 = GetPositionIndex(animationTime);
		int p1 = p0 + 1;
		
		float scaleFactor = GetScaleFactor(m_Positions[p0].timeStamp, m_Positions[p1].timeStamp, animationTime);
        glm::vec3 finalPosition = glm::mix(m_Positions[p0].position, m_Positions[p1].position, scaleFactor);

        return glm::translate(glm::mat4(1.0f), finalPosition);
	}

	glm::mat4 Bone::InterpolateRotation(float animationTime)
	{
		if (m_NumRotations == 1)
		{
			auto rotation = glm::normalize(m_Rotations[0].orientation);
            return glm::toMat4(rotation);
		}

		int p0 = GetRotationIndex(animationTime);
		int p1 = p0 + 1;
		
		float scaleFactor = GetScaleFactor(m_Rotations[p0].timeStamp, m_Rotations[p1].timeStamp, animationTime);
		glm::quat finalRotation = glm::slerp(m_Rotations[p0].orientation, m_Rotations[p1].orientation, scaleFactor);

		finalRotation = glm::normalize(finalRotation);
		return glm::toMat4(finalRotation);
	}

	glm::mat4 Bone::InterpolateScaling(float animationTime)
	{
		if (m_NumScalings == 1)
			return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

		int p0 = GetScaleIndex(animationTime);
		int p1 = p0 + 1;
		
		float scaleFactor = GetScaleFactor(m_Scales[p0].timeStamp, m_Scales[p1].timeStamp, animationTime);
		glm::vec3 finalScale = glm::mix(m_Scales[p0].scale, m_Scales[p1].scale, scaleFactor);

		return glm::scale(glm::mat4(1.0f), finalScale);
	}
}
