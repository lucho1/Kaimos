#include "kspch.h"
#include "Scene.h"

#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"

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
		Renderer::CreateDefaultMaterial();
	}

	Scene::Scene(const std::string& name) : m_Name(name)
	{
		m_PrimaryCamera = {};
	}
	


	// ----------------------- Public/Private Scene Methods -----------------------------------------------
	void Scene::RenderScene()
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto ent : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(ent);
			if (transform.EntityActive)
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)ent);
		}
	}

	void Scene::OnUpdateEditor(Timestep dt, const Camera& camera)
	{
		KS_PROFILE_FUNCTION();

		// -- Render --
		Renderer2D::BeginScene(camera);
		RenderScene();
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
			Renderer2D::BeginScene(m_PrimaryCamera.GetComponent<CameraComponent>(), m_PrimaryCamera.GetComponent<TransformComponent>());
			RenderScene();
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
			Renderer2D::BeginScene(camera_entity.GetComponent<CameraComponent>(), camera_entity.GetComponent<TransformComponent>());
			RenderScene();
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
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component) const
	{
		component.SetMaterial(Renderer::GetDefaultMaterialID());
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component) const
	{
	}
}
