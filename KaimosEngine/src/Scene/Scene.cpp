#include "kspch.h"
#include "Scene.h"

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

	struct TransformComponent
	{
		glm::mat4 transform;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform) : transform(transform) {}

		operator const glm::mat4& () const { return transform; } // This allows to use TransformComponent as a matrix (overloading magic), it implicitly makes the cast
		operator glm::mat4& () { return transform; }
	};
	struct MeshComponent {};

	Scene::Scene()
	{
		// -- To operate with entities: --
		entt::entity entity = m_Registry.create();
		// To add a component type into the registry (and it actually returns the comp. if you want to store it), this case a transform comp. with transform initialized to glm::mat4(1.0f)
		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f)); // This component is created on fly (don't need to register them on time)
	}

	Scene::~Scene()
	{
	}

}