#pragma once

#include "Calibur/Core/Base.h"
#include "Calibur/Animation/Bone.h"
#include "Calibur/Renderer/Mesh.h"

namespace Calibur
{
	struct AssimpNodeData
	{
		glm::mat4 transformation;
		std::string name;
		int childrenCount;
		std::vector<AssimpNodeData> children;
	};

	class Animation
	{
	public:
		Animation() = default;
		~Animation() = default;

		Animation(const std::string& animationPath, Ref<Mesh> mesh);

		float GetDuration() const { return m_Duration; }
		float GetTicksPerSecond() const { return m_TicksPerSecond; }
		const std::vector<Bone>& GetBones() const { return m_Bones; }
		AssimpNodeData& GetRootNode() { return m_RootNode; }

		Bone* GetBone(const std::string& name);
		std::unordered_map<std::string, BoneInfo>& GetBoneInfoMap() { return m_BoneInfoMap; }
	private:
		void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);
		void ReadMissingBones(const aiAnimation* animation, Ref<Mesh> mesh);
	private:
		float m_Duration;
		float m_TicksPerSecond;
		std::vector<Bone> m_Bones;
		AssimpNodeData m_RootNode;
		std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
	};
}
