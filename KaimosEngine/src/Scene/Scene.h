#ifndef _SCENE_H_
#define _SCENE_H_

#include "Core/Utils/Time/Timestep.h"
#include "Renderer/Cameras/Camera.h"
#include "Renderer/Cameras/CameraController.h"

#include <entt.hpp>

namespace Kaimos {

	namespace Resources { class ResourceModel; }

	class Light;
	class PointLight;
	class Mesh;
	class Entity;
	class TransformComponent;
	class CameraComponent;

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
		void OnUpdateEditor(Timestep dt);
		void OnUpdateRuntime(Timestep dt);

		void RenderFromCamera(Timestep dt, const Entity& camera_entity);
		void SetViewportSize(uint width, uint height);
		void ConvertModelIntoEntities(const Ref<Resources::ResourceModel>& model);

		// --- Public Entities Methods ---
		Entity CreateEntity(const std::string& name = "unnamed", uint entity_id = 0);
		void DestroyEntity(Entity entity);
		void UpdateMeshAndSpriteComponentsVertices(uint material_id);

	public:

		// --- Getters/Setters ---
		static Entity GetPrimaryCamera();
		void SetPrimaryCamera(Entity new_camera_entity);
		void UnsetPrimaryCamera();

		void SetGlobalCurrentScene(Ref<Scene> scene);
		static CameraController& GetEditorCamera();
		static float GetCameraFOV();
		static float GetCameraAR();
		static float GetCameraOrthoSize();
		static glm::vec2 GetCameraPlanes();

		inline const std::string GetName()				const { return m_Name; }
		inline const std::string GetPath()				const { return m_Path; }
		inline void SetName(const std::string& name)	{ m_Name = name; }
		inline void SetPath(const std::string& path)	{ m_Path = path; }

	private:

		// --- Private Scene Lights Methods ---
		std::vector<std::pair<Ref<Light>, glm::vec3>> GetSceneDirLights();
		std::vector<std::pair<Ref<PointLight>, glm::vec3>> GetScenePointLights();

		// --- Private Scene Rendering Methods ---
		void BeginScene(const Camera& camera, const glm::vec3& camera_pos, bool scene3D);
		void BeginScene(const CameraComponent& camera_component, const TransformComponent& transform_component, bool scene3D);

		void RenderSprites(Timestep dt);
		void RenderMeshes(Timestep dt);

		// --- Private Scene Methods ---
		void ConvertMeshIntoEntities(const Ref<Mesh>& mesh);

		// --- Private Scene Entities Methods ---
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
