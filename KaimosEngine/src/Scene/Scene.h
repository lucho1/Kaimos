#ifndef _SCENE_H_
#define _SCENE_H_

#include "entt.hpp"

namespace Kaimos {

	class Scene
	{
	public:

		Scene();
		~Scene();

	private:

		entt::registry m_Registry;
	};
}
#endif //_SCENE_H_