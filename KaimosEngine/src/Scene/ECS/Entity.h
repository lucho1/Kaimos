#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "Scene/Scene.h"
#include <entt.hpp>

namespace Kaimos {

	class Entity
	{
	public:

		// --- Public Class Methods ---
		Entity() = default;
		Entity(const Entity& entity) = default;
		Entity(entt::entity entity, Scene* scene) : m_EntityID(entity), m_Scene(scene) {}
		Entity(uint entity_id, Scene* scene) : m_EntityID((entt::entity)entity_id), m_Scene(scene) {}

	public:

		// --- Getters/Setters ---
		inline uint GetID()								const	{ return (uint)m_EntityID; }
		inline bool IsActive()							const	{ return m_Active; }
		inline void SetActive(bool active)						{ m_Active = active; }


	// -- (Exception) vars here for readability, many templated long functions below --
	private:
		
		entt::entity m_EntityID = entt::null;
		bool m_Active = true;

		Scene* m_Scene = nullptr;


	public:
		
		// --- Operators ---
		//inline operator entt::entity()				const { return m_EntityID; }
		inline operator bool()							const { return m_EntityID != entt::null; }

		inline bool operator !=(const Entity& other)	const { return !(*this == other); }												// Using the operator below, that's elegant :)
		inline bool operator ==(const Entity& other)	const { return m_EntityID == other.m_EntityID && m_Scene == other.m_Scene; }


		// --- Public Component Methods ---
		template<typename T>
		bool HasComponent()								const { return m_Scene->m_Registry.has<T>(m_EntityID); }

		template<typename T, typename...Args>
		T& AddComponent(Args&&... args) const
		{
			// Variatic template - "Args&& ... args" are all of the variables we are passing in, then std::forward makes them to not to be "unpacked" (handled) in here,
			// but to forward them all the way to the emplace() function (to entt, basically, which takes the same as we are passing, if you F12 in emplace())
			if (!HasComponent<T>())
			{
				T& comp = m_Scene->m_Registry.emplace<T>(m_EntityID, std::forward<Args>(args)...);
				m_Scene->OnComponentAdded<T>(*this, comp);
				return comp;
			}
			else
				KS_ERROR_AND_ASSERT("Entity already has the Component to Add");
		}

		template<typename T>
		T& GetComponent() const
		{
			if (HasComponent<T>())
				return m_Scene->m_Registry.get<T>(m_EntityID);
			else
				KS_ERROR_AND_ASSERT("Entity does not has the Component to Get");
		}

		template<typename T>
		void RemoveComponent() const
		{
			if (HasComponent<T>())
				m_Scene->m_Registry.remove<T>(m_EntityID);
			else
				KS_ERROR_AND_ASSERT("Entity does not has the Component to Remove");
		}
	};
}

#endif //_ENTITY_H_
