#include "kspch.h"
#include "Scene.h"

#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "Renderer/Renderer2D.h"

#include "Core/Utils/Maths/RandomGenerator.h"
#include <glm/glm.hpp>

namespace Kaimos {

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



	// ----------------------- Public Scene Methods -------------------------------------------------------
	void Scene::OnUpdateEditor(Timestep dt, const Camera& camera)
	{
		KS_PROFILE_FUNCTION();

		// -- Render --
		Renderer2D::BeginScene(camera);

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto ent : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(ent);
			if (transform.EntityActive)
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)ent);
		}

		Renderer2D::EndScene();
	}

	// TODO: Remake this in the Camera Rework
	void Scene::OnUpdateRuntime(Timestep dt)
	{
		KS_PROFILE_FUNCTION();

		// -- Scripts (should be in Scene::OnScenePlay() or similar, upon pressing engine's play button) --
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& component) // Lambda that will be called for each of the NativeScriptComponent
			{
				if (!component.EntityInstance)
				{
					component.EntityInstance = component.InstantiateScript();
					component.EntityInstance->m_Entity = Entity(entity, this);
					component.EntityInstance->OnCreate();
				}

				component.EntityInstance->OnUpdate(dt);
			});


		// -- Render --
		auto view = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto ent : view)
		{
			auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(ent);

			if (camera.Primary)
			{
				Renderer2D::BeginScene(camera, transform);
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto ent : group)
				{
					auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(ent);
					Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)ent);
				}

				Renderer2D::EndScene();
				break;
			}
		}
	}

	void Scene::SetViewportSize(uint width, uint height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// -- Resize Cameras with non-fixed AR --
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& camComp = view.get<CameraComponent>(entity);
			if (!camComp.FixedAspectRatio)
				camComp.Camera.SetViewport(width, height);
		}
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
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}

		return {};
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
		component.Camera.SetViewport(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component) const
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component) const
	{
	}
}
