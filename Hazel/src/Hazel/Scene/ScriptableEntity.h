#pragma once

#include "Hazel/Scene/Entity.h"
#include "Hazel/Core/Base.h"

namespace Hazel
{
	class ScriptableEntity 
	{
	public:
		virtual ~ScriptableEntity() = default;

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(TimeStep ts) {}
	private:
		Entity m_Entity;

		friend class Scene;
	};
}
