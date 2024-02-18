#pragma once

#include "Calibur/Animation/Animation.h"

namespace Calibur
{
	class Animator
	{
	public:
		Animator(Ref<Animation> animation);
		~Animator() = default;
		
		void PlayAnimation(Ref<Animation> pAnimation);
		void UpdateAnimation(float dt);
		void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
		
		std::vector<glm::mat4>& GetFinalBoneMatrices() { return m_FinalBoneMatrices; }
	private:
		std::vector<glm::mat4> m_FinalBoneMatrices;
		Ref<Animation> m_CurrentAnimation;
		float m_CurrentTime;
		float m_DeltaTime;
	};
}
