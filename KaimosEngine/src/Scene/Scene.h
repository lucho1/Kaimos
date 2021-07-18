#ifndef _SCENE_H_
#define _SCENE_H_

#include "Core/Utils/Time/Timestep.h"
#include "Renderer/Cameras/Camera.h"

#include <entt.hpp>

namespace Kaimos {

	namespace Resources { class ResourceModel; }
	class Mesh;
	class Entity;

	class Scene
	{
		friend class Entity;
		friend class ScenePanel;
		friend class SceneSerializer;
	public:

		// --- Public Class Methods ---
		Scene();
		Scene(const std::string& name);
		~Scene() = default;

		// --- Public Scene Methods ---
		void OnUpdateEditor(Timestep dt, const Camera& camera);
		void OnUpdateRuntime(Timestep dt);
		void RenderFromCamera(Timestep dt, const Entity& camera_entity);
		void SetViewportSize(uint width, uint height);
		void ConvertModelIntoEntities(const Ref<Resources::ResourceModel>& model);

		// --- Public Entities Methods ---
		Entity CreateEntity(const std::string& name = "unnamed", uint entity_id = 0);
		void DestroyEntity(Entity entity);
		void UpdateMeshComponentsVertices(uint material_id);

	public:

		// --- Getters/Setters ---
		Entity GetPrimaryCamera();
		void SetPrimaryCamera(Entity new_camera_entity);
		void UnsetPrimaryCamera();

		inline const std::string GetName()				const { return m_Name; }
		inline const std::string GetPath()				const { return m_Path; }
		inline void SetName(const std::string& name)	{ m_Name = name; }
		inline void SetPath(const std::string& path)	{ m_Path = path; }

	private:

		// --- Private Scene Methods ---
		void RenderSprites();
		void RenderMeshes();
		void ConvertMeshIntoEntities(const Ref<Mesh>& mesh);

		// --- Private Entities Methods ---
		template<typename T>
		void OnComponentAdded(Entity entity, T& component) const;

	private:

		std::string m_Name = "KaimosUnnamedScene";
		std::string m_Path = "";
		entt::registry m_Registry = {};
		uint m_ViewportWidth = 0, m_ViewportHeight = 0;
	};
}
#endif //_SCENE_H_
