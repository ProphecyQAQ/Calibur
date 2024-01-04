#pragma once

#include "Calibur/Scene/Entity.h"
#include "Calibur/Core/Base.h"

namespace Calibur
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
