#ifndef _SCRIPTABLE_ENTITY_H_
#define _SCRIPTABLE_ENTITY_H_

#include "Entity.h"

namespace Kaimos {

	class ScriptableEntity
	{
		friend class Scene;
	public:

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

	private:

		Entity m_Entity;
	};
}

#endif //_SCRIPTABLE_ENTITY_H_