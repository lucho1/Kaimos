#include "kspch.h"
#include "Scene.h"

#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/Light.h"

#include "Core/Resources/ResourceManager.h"
#include "Core/Resources/Resource.h"
#include "Core/Resources/ResourceModel.h"
#include "Core/Utils/Maths/RandomGenerator.h"

#include <glm/glm.hpp>


namespace Kaimos {

	static Entity m_PrimaryCamera;
	// entt::entity entity = registry.create();						-- To create entities in the registry
	// registry.clear();											-- To clear the registry (remove all in it)

	// registry.emplace<Comp>(ent, CompInits);						-- To add a component in the registry (returns the component!) - CompInits are initializators (Transform -> TransfMatrix)
	// registry.remove<Comp>(ent);									-- Remove a list of components from an entity
	// registry.get<Comp>(ent);										-- To get a given component from an entity

	// if (registry.has<Comp>(ent)) registry.get<Comp>(ent)			-- Retrieves a component if exists in "ent" (get() returns it) 

	// registry.on_construct<Comp>().connect(func);					-- Callbacks to events (also: on_destroy, on_update...) - "func" = func(entt::registry&, entt::entity)

	// view = registry.view<Comp>(); for(ent : view) {}				-- To iterate through all entities with a given component
	// group = registry.group<Comp1>(entt::get<Comp2>);				-- To retrieve entities with 2 given components.
	//																-- If "group" is iterated, we could code "auto&[c1, c2] = group.get<Comp1, Comp2>(ent)" (c1, c2 being variables)



	// ----------------------- Public Class Methods -------------------------------------------------------
	Scene::Scene()
	{
		m_PrimaryCamera = {};
	}

	Scene::Scene(const std::string& name) : m_Name(name)
	{
		m_PrimaryCamera = {};
	}
	


	// ----------------------- Public/Private Scene Methods -----------------------------------------------
	std::vector<Ref<Light>> Scene::GetSceneDirLights()
	{
		std::vector<Ref<Light>> dir_lights;
		auto dirlights_group = m_Registry.group<DirectionalLightComponent>(entt::get<TransformComponent>);
		dir_lights.reserve(dirlights_group.size());

		for (auto ent : dirlights_group)
		{
			auto& [light, transform] = dirlights_group.get<DirectionalLightComponent, TransformComponent>(ent);
			if (transform.EntityActive && light.Visible)
				dir_lights.push_back(light.Light);
		}

		return dir_lights;
	}

	std::vector<Ref<PointLight>> Scene::GetScenePointLights()
	{
		std::vector<Ref<PointLight>> point_lights;
		auto pointlights_group = m_Registry.group<PointLightComponent>(entt::get<TransformComponent>);
		point_lights.reserve(pointlights_group.size());

		for (auto ent : pointlights_group)
		{
			auto& [light, transform] = pointlights_group.get<PointLightComponent, TransformComponent>(ent);
			if (transform.EntityActive && light.Visible)
				point_lights.push_back(light.Light);
		}

		return point_lights;
	}

	void Scene::BeginScene(const Camera& camera, bool scene3D)
	{
		std::vector<Ref<Light>> dir_lights = GetSceneDirLights();
		std::vector<Ref<PointLight>> plights = GetScenePointLights();

		glm::mat4 view_proj = camera.GetViewProjection();
		scene3D ? Renderer3D::BeginScene(view_proj, dir_lights, plights) : Renderer2D::BeginScene(view_proj, dir_lights, plights);
	}

	void Scene::BeginScene(const CameraComponent& camera_component, const TransformComponent& transform_component, bool scene3D)
	{
		std::vector<Ref<Light>> dir_lights = GetSceneDirLights();
		std::vector<Ref<PointLight>> plights = GetScenePointLights();

		glm::mat4 view_proj = camera_component.Camera.GetProjection() * glm::inverse(transform_component.GetTransform());
		scene3D ? Renderer3D::BeginScene(view_proj, dir_lights, plights) : Renderer2D::BeginScene(view_proj, dir_lights, plights);
	}

	void Scene::RenderSprites(Timestep dt)
	{
		KS_PROFILE_FUNCTION();
		auto sprite_group = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
		for (auto ent : sprite_group)
		{
			auto& [sprite, transform] = sprite_group.get<SpriteRendererComponent, TransformComponent>(ent);
			if (transform.EntityActive)
				Renderer2D::DrawSprite(dt, transform.GetTransform(), sprite, (int)ent);
		}
	}

	void Scene::RenderMeshes(Timestep dt)
	{
		KS_PROFILE_FUNCTION();
		auto mesh_group = m_Registry.group<TransformComponent>(entt::get<MeshRendererComponent>);
		for (auto ent : mesh_group)
		{
			auto& [transform, mesh] = mesh_group.get<TransformComponent, MeshRendererComponent>(ent);
			if (transform.EntityActive)
				Renderer3D::DrawMesh(dt, transform.GetTransform(), mesh, (int)ent);
		}
	}

	void Scene::OnUpdateEditor(Timestep dt, const Camera& camera)
	{
		KS_PROFILE_FUNCTION();

		// -- Render Meshes --
		BeginScene(camera, true);
		RenderMeshes(dt);
		Renderer3D::EndScene();

		// -- Render Sprites --
		BeginScene(camera, false);
		RenderSprites(dt);
		Renderer2D::EndScene();
	}

	void Scene::OnUpdateRuntime(Timestep dt)
	{
		KS_PROFILE_FUNCTION();

		// -- Scripts (should be in Scene::OnScenePlay() or similar, upon pressing engine's play button) --
		/*m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& component) // Lambda that will be called for each of the NativeScriptComponent
			{
				if (!component.EntityInstance)
				{
					component.EntityInstance = component.InstantiateScript();
					component.EntityInstance->m_Entity = Entity(entity, this);
					component.EntityInstance->OnCreate();
				}

				component.EntityInstance->OnUpdate(dt);
			});*/

		// -- Render --
		static bool primary_camera_warn = false;
		if (m_PrimaryCamera)
		{
			BeginScene(m_PrimaryCamera.GetComponent<CameraComponent>(), m_PrimaryCamera.GetComponent<TransformComponent>(), true);
			RenderMeshes(dt);
			Renderer3D::EndScene();

			BeginScene(m_PrimaryCamera.GetComponent<CameraComponent>(), m_PrimaryCamera.GetComponent<TransformComponent>(), false);
			RenderSprites(dt);
			Renderer2D::EndScene();
			primary_camera_warn = false;
		}
		else if(!primary_camera_warn)
		{
			primary_camera_warn = true;
			KS_EDITOR_WARN("No Primary Camera Setted!");
		}
	}

	void Scene::RenderFromCamera(Timestep dt, const Entity& camera_entity)
	{
		// -- Render --
		if (camera_entity && camera_entity.HasComponent<CameraComponent>())
		{
			BeginScene(m_PrimaryCamera.GetComponent<CameraComponent>(), m_PrimaryCamera.GetComponent<TransformComponent>(), true);
			RenderMeshes(dt);
			Renderer3D::EndScene();

			BeginScene(camera_entity.GetComponent<CameraComponent>(), camera_entity.GetComponent<TransformComponent>(), false);
			RenderSprites(dt);
			Renderer2D::EndScene();
		}
	}

	void Scene::SetViewportSize(uint width, uint height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// -- Resize Cameras with non-fixed AR --
		//auto view = m_Registry.view<CameraComponent>();
		//for (auto entity : view)
		//{
		//	auto& camComp = view.get<CameraComponent>(entity);
		//	if (!camComp.FixedAspectRatio)
		//		camComp.Camera.SetViewport(width, height);
		//}
	}

	void Scene::ConvertModelIntoEntities(const Ref<Resources::ResourceModel>& model)
	{
		if (model && Resources::ResourceManager::ModelExists(model->GetID()))
			ConvertMeshIntoEntities(model->GetRootMesh());
	}

	void Scene::ConvertMeshIntoEntities(const Ref<Mesh>& mesh)
	{
		if (!mesh || !Resources::ResourceManager::MeshExists(mesh->GetID()))
			return;

		MeshRendererComponent& mesh_comp = CreateEntity(mesh->GetName()).AddComponent<MeshRendererComponent>();
		mesh_comp.SetMesh(mesh->GetID());
		mesh_comp.SetMaterial(mesh->GetMaterialID());

		const std::vector<Ref<Mesh>>& sub_meshes = mesh->GetSubmeshes();
		for (const Ref<Mesh>& m : sub_meshes)
			ConvertMeshIntoEntities(m);
	}


	
	// ----------------------- Public Entities Methods ---------------------------------------------------
	Entity Scene::CreateEntity(const std::string& name, uint entity_id)
	{
		KS_PROFILE_FUNCTION();

		uint id = entity_id == 0 ? (uint)Random::GetRandomInt() : entity_id;
		Entity entity = { m_Registry.create(entt::entity{id}), this };

		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>(name);
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy((entt::entity)entity.GetID());
	}

	void Scene::UpdateMeshAndSpriteComponentsVertices(uint material_id)
	{
		KS_PROFILE_FUNCTION();
		auto mesh_view = m_Registry.view<MeshRendererComponent>();
		for (auto ent : mesh_view)
		{
			MeshRendererComponent& mesh_comp = mesh_view.get<MeshRendererComponent>(ent);
			if(mesh_comp.MaterialID == material_id)
				mesh_comp.UpdateModifiedVertices();
		}

		auto sprite_view = m_Registry.view<SpriteRendererComponent>();
		for (auto ent : sprite_view)
		{
			SpriteRendererComponent& sprite_comp = sprite_view.get<SpriteRendererComponent>(ent);
			if (sprite_comp.SpriteMaterialID == material_id)
				sprite_comp.UpdateVertices();
		}
	}



	// ----------------------- Getters/Setters -----------------------------------------------------------
	Entity Scene::GetPrimaryCamera()
	{
		if(m_PrimaryCamera && m_PrimaryCamera.GetComponent<TransformComponent>().EntityActive)
			return m_PrimaryCamera;

		return {};
	}

	void Scene::SetPrimaryCamera(Entity new_camera_entity)
	{
		if (!new_camera_entity || !new_camera_entity.HasComponent<CameraComponent>())
			return;

		if (m_PrimaryCamera && new_camera_entity == m_PrimaryCamera)
			return;

		if (m_PrimaryCamera)
			m_PrimaryCamera.GetComponent<CameraComponent>().Primary = false;

		new_camera_entity.GetComponent<CameraComponent>().Primary = true;
		m_PrimaryCamera = new_camera_entity;
	}

	void Scene::UnsetPrimaryCamera()
	{
		if (m_PrimaryCamera)
			m_PrimaryCamera.GetComponent<CameraComponent>().Primary = false;

		m_PrimaryCamera = {};
	}

	

	// ----------------------- Private Entities Methods --------------------------------------------------
	// On Component Added
	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component) const
	{
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component) const
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component) const
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component) const
	{
		component.Camera.SetViewport(component.Camera.GetViewportSize().x, component.Camera.GetViewportSize().y);
	}

	template<>
	void Scene::OnComponentAdded<DirectionalLightComponent>(Entity entity, DirectionalLightComponent& component) const
	{
	}

	template<>
	void Scene::OnComponentAdded<PointLightComponent>(Entity entity, PointLightComponent& component) const
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component) const
	{
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component) const
	{
		component.RemoveMaterial();
	}

	template<>
	void Scene::OnComponentAdded<MeshRendererComponent>(Entity entity, MeshRendererComponent& component) const
	{
		component.RemoveMaterial();
	}


	// On Component Removed
	//template<>
	//void Scene::OnComponentRemoved<LightComponent>(Entity entity, LightComponent& component) const
	//{
	//	if (component.Light->GetLightType() == LightType::POINTLIGHT)
	//	{
	//		Ref<PointLight> plight_ref = Ref<PointLight>(static_cast<PointLight*>(component.Light.get()));
	//		PointLight* plight = static_cast<PointLight*>(component.Light.get());
	//
	//		plight_ref.reset();
	//		delete plight;
	//	}
	//	
	//	component.Light.reset();
	//}
}
