#include "kspch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "Renderer/Renderer2D.h"

#include <glm/glm.hpp>

namespace Kaimos {

		//// -- To operate with entities: --
		//entt::entity entity = m_Registry.create();
		//// To add a component type into the registry (and it actually returns the comp. if you want to store it), this case a transform comp. with transform initialized to glm::mat4(1.0f)
		//m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f)); // This component is created on fly (don't need to register them on time)
		//// To remove a list of components from an entity, this case TransformComponent:
		//m_Registry.remove<TransformComponent>(entity);
		//// To remove everything from the registry:
		//m_Registry.clear();
		//// To retreive a component:
		//if(m_Registry.has<TransformComponent>(entity))
		//	TransformComponent& transf = m_Registry.get<TransformComponent>(entity);
		//
		//// To make callbacks to certain events:
		//m_Registry.on_construct<TransformComponent>().connect(/*function*/); // Function shall be func(entt::registry&, entt::entity)
		//m_Registry.on_destroy<TransformComponent>().connect(/*function*/);
		//m_Registry.on_update<TransformComponent>().connect(/*function*/);
		//
		//// To iterate through all entities with transform components:
		//auto view = m_Registry.view<TransformComponent>();
		//for (auto ent : view)
		//{
		//	view.get<MeshComponent>(ent); // This gets the mesh component of the current component's entity
		//}
		//
		//// This also works to retrieve groups of components and other components that their entities have:
		//auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
		//for (auto ent : group)
		//{
		//	auto&[transform, mesh] = group.get<TransformComponent, MeshComponent>(ent);
		//}


	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}
	
	void Scene::OnUpdateEditor(Timestep dt, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto ent : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(ent);
			Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)ent);
		}

		//Kaimos::Renderer2D::DrawLine(camera.GetProjection(), { -0.3f, -0.4f, 0.0f }, { -0.3f, 0.4f, 0.0f }, 3.0f, { 0.2f, 0.2f, 0.2f, 1.0f });
		Kaimos::Renderer2D::DrawLine(camera.GetViewProj(), { 5.0f, 5.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 100.0f, { 1.0f, 0.0f, 0.0f, 1.0f });
		Kaimos::Renderer2D::DrawLine(camera.GetViewProj(), { -5.0f, 0.0f, 0.0f }, { 5.0f, 0.0f, 0.0f }, 100.0f, { 0.0f, 1.0f, 0.0f, 1.0f });

		Renderer2D::EndScene();
	}

	void Scene::OnUpdateRuntime(Timestep dt)
	{
		// Scripts --> This should happen in Scene::OnScenePlay() or similar (pressing the engine's play button)
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

		// Render
		Camera* mainCam = nullptr;
		glm::mat4 camTransform;

		auto view = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto ent : view)
		{
			auto&[transform, camera] = view.get<TransformComponent, CameraComponent>(ent);
			
			if (camera.Primary)
			{
				mainCam = &camera.Camera;
				camTransform = transform.GetTransform();
				break;
			}
		}

		if (mainCam)
		{
			Renderer2D::BeginScene(*mainCam, camTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto ent : group)
			{
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(ent);
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)ent);
			}

			Renderer2D::EndScene();
		}
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>(name);
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
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

	void Scene::SetViewportSize(uint width, uint height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize cameras which have non-fixed AR
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& camComp = view.get<CameraComponent>(entity);
			if (!camComp.FixedAspectRatio)
				camComp.Camera.SetViewportSize(width, height);
		}
	}


	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}	

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}
}
