#ifndef _SCENE_H_
#define _SCENE_H_

#include "Core/Time/Timestep.h"
#include "Renderer/Cameras/EditorCamera.h"

#include <entt.hpp>

namespace Kaimos {

	class Entity;

	class Scene
	{
		friend class Entity;
		friend class ScenePanel;
		friend class SceneSerializer;
	public:

		Scene();
		~Scene();

		void OnUpdateRuntime(Timestep dt);
		void OnUpdateEditor(Timestep dt, EditorCamera& camera);

		void SetViewportSize(uint width, uint height);

		Entity CreateEntity(const std::string& name = "unnamed");
		void DestroyEntity(Entity entity);

		Entity GetPrimaryCamera();

	private:

		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

	private:

		entt::registry m_Registry;
		uint m_ViewportWidth = 0, m_ViewportHeight = 0;
	};
}
#endif //_SCENE_H_
