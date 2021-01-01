#ifndef _SCENE_H_
#define _SCENE_H_

#include "entt.hpp"
#include "Core/Time/Timestep.h"

namespace Kaimos {

	class Entity;

	class Scene
	{
		friend class Entity;
	public:

		Scene();
		~Scene();

		void OnUpdate(Timestep dt);

		void SetViewportSize(uint width, uint height);

		Entity CreateEntity(const std::string& name = "unnamed");

	private:

		entt::registry m_Registry;
		uint m_ViewportWidth = 0, m_ViewportHeight = 0;
	};
}
#endif //_SCENE_H_