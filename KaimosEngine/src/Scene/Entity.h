#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "Scene.h"
#include "entt.hpp"

namespace Kaimos {

	class Entity
	{
	public:

		Entity() = default;
		Entity(entt::entity entity, Scene* scene) : m_Entity(entity), m_Scene(scene) {}
		Entity(const Entity& entity) = default;

		// Variatic template - "Args&& ... args" are all of the variables we are passing in, then std::forward makes them to not to be "unpacked" (handled) in here,
		// but to forward them all the way to the emplace() function (to entt, basically, which takes the same as we are passing, if you F12 in emplace())
		template<typename T, typename...Args>
		T& AddComponent(Args&&... args)
		{
			KS_ENGINE_ASSERT(!HasComponent<T>(), "Entity already has component");
			T& comp = m_Scene->m_Registry.emplace<T>(m_Entity, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, comp);
			return comp;
		}

		template<typename T>
		T& GetComponent()
		{
			KS_ENGINE_ASSERT(HasComponent<T>(), "Entity doesn't has component");
			return m_Scene->m_Registry.get<T>(m_Entity);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.has<T>(m_Entity);
		}

		template<typename T>
		void RemoveComponent()
		{
			KS_ENGINE_ASSERT(HasComponent<T>(), "Entity doesn't has component");
			return m_Scene->m_Registry.remove<T>(m_Entity);
		}

		bool operator ==(const Entity& other)	const { return m_Entity == other.m_Entity && m_Scene == other.m_Scene; }
		bool operator !=(const Entity& other)	const { return !(*this == other); } // You are using the above operator, that's elegant :)

		operator bool()							const { return m_Entity != entt::null; }
		operator entt::entity()					const { return m_Entity; }
		operator uint()							const { return (uint)m_Entity; }

		uint GetID()							const { return(uint)m_Entity; }

	private:

		entt::entity m_Entity = entt::null;
		Scene* m_Scene = nullptr;
	};
}

#endif // _ENTITY_H_