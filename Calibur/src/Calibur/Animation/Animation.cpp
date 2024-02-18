#include "hzpch.h"
#include "Calibur/Animation/Animation.h"

namespace Calibur
{
	namespace Utils
	{
		glm::mat4 ConvertAiMat4ToGlm(const aiMatrix4x4& aiMat);
	}

	static const uint32_t s_AssimpFlag = 			
			aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
			aiProcess_Triangulate |             // Make sure we're triangles
			aiProcess_SortByPType |             // Split meshes by primitive type
			aiProcess_GenNormals |              // Make sure we have legit normals
			aiProcess_GenUVCoords |             // Convert UVs if required
//			aiProcess_OptimizeGraph |
			aiProcess_OptimizeMeshes |          // Batch draws where possible
			aiProcess_JoinIdenticalVertices |
			aiProcess_LimitBoneWeights |        // If more than N (=4) bone weights, discard least influencing bones and renormalise sum to 1
			aiProcess_GlobalScale |             // e.g. convert cm to m for fbx import (and other formats where cm is native)
//			aiProcess_PopulateArmatureData |    // not currently using this data
			aiProcess_ValidateDataStructure;    // Validation 

	Animation::Animation(const std::string& animationPath, Ref<Mesh> mesh)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, s_AssimpFlag);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			HZ_CORE_ERROR("Assimp import error::{0}", importer.GetErrorString());
			HZ_CORE_ASSERT(false);
		}

		auto animation = scene->mAnimations[0];
		m_Duration = animation->mDuration;
		m_TicksPerSecond = animation->mTicksPerSecond;

		ReadHeirarchyData(m_RootNode, scene->mRootNode);
		ReadMissingBones(animation, mesh);
	}

	Bone* Animation::GetBone(const std::string& name)
	{
		auto it = std::find_if(m_Bones.begin(), m_Bones.end(), [&](const Bone& bone) {
			return bone.GetBoneName() == name;
		});

		if (it != m_Bones.end()) return &(*it);
		else return nullptr;
	}

	void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
	{
		dest.name = src->mName.data;
		dest.transformation = Utils::ConvertAiMat4ToGlm(src->mTransformation);
		dest.childrenCount = src->mNumChildren;

		for (size_t i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData data;
			ReadHeirarchyData(data, src->mChildren[i]);
			dest.children.push_back(data);
		}
	}

	void Animation::ReadMissingBones(const aiAnimation* animation, Ref<Mesh> mesh)
	{
		uint32_t size = animation->mNumChannels;

		auto& boneInfoMap = mesh->GetBoneInfoMap();
		uint32_t& boneCount = mesh->GetBoneCount();

		for (size_t i = 0; i < size; i++)
		{
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.C_Str();

			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].BoneIndex = boneCount;
				boneCount ++;
			}
			m_Bones.push_back(Bone(boneName, boneInfoMap[boneName].BoneIndex, channel));
		}
		m_BoneInfoMap = boneInfoMap;
	}
}
