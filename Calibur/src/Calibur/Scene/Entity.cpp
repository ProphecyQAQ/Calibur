#include "hzpch.h"
#include "Calibur/Scene/Entity.h"

namespace Calibur
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

	bool Entity::HasParent()
	{
		UUID& parentID = GetComponent<RelationshipComponent>().ParentID;

		if (m_Scene->m_EntityMap.find(parentID) != m_Scene->m_EntityMap.end())
		{
			return true;
		}
		return false;
	}

}
