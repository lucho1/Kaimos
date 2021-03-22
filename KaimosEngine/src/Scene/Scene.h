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

		// --- Public Class Methods ---
		Scene() = default;
		~Scene() = default;

		// --- Public Scene Methods ---
		void OnUpdateEditor(Timestep dt, EditorCamera& camera);
		void OnUpdateRuntime(Timestep dt);

		void SetViewportSize(uint width, uint height);

		// --- Public Entities Methods ---
		Entity CreateEntity(const std::string& name = "unnamed");
		void DestroyEntity(Entity entity);

		Entity GetPrimaryCamera();

	private:

		// --- Private Entities Methods ---
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

	private:

		entt::registry m_Registry = {};
		uint m_ViewportWidth = 0, m_ViewportHeight = 0;
	};
}
#endif //_SCENE_H_
