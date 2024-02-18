#include "hzpch.h"

#include "Calibur/Animation/Animator.h"

namespace Calibur 
{

	Animator::Animator(Ref<Animation> animation)
		:m_CurrentAnimation(animation), m_CurrentTime(0.0f)
	{
		m_FinalBoneMatrices.reserve(100);
		for (size_t i = 0; i < 100; i++)
		{
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
		}
	}

	void Animator::UpdateAnimation(float dt)
	{
		//dt = 1.53595912;
		m_DeltaTime = dt;
		if (m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}

	void Animator::PlayAnimation(Ref<Animation> pAnimation)
	{
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = 0.0f;
	}

	void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;
		
		Bone* bone = m_CurrentAnimation->GetBone(nodeName);

		if (bone)
		{
			bone->Update(m_CurrentTime);
			nodeTransform = bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto& boneInfoMap = m_CurrentAnimation->GetBoneInfoMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].BoneIndex;
			glm::mat4 offset = boneInfoMap[nodeName].OffsetMatrix;
			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
	}
}
