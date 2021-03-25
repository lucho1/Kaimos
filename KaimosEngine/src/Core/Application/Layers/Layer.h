#ifndef _LAYER_H_
#define _LAYER_H_

#include "kspch.h"
#include "Core/Core.h"
#include "Core/Utils/Time/Timestep.h"
#include "Core/Application/Events/Event.h"

namespace Kaimos {

	class Layer
	{
	public:

		// --- Public Class Methods ---
		Layer(const std::string& name = "Unnamed Layer") : m_Name(name) {}
		virtual ~Layer() = default;

		// --- Public Layer Methods ---
		virtual void OnAttach()				{} // Added to layer stack
		virtual void OnDetach()				{} // Removed from layer stack
		virtual void OnUpdate(Timestep dt)	{}
		virtual void OnUIRender()			{}
		virtual void OnEvent(Event& event)	{}

		// --- Getters/Setters ---
		inline const std::string& GetName() const { return m_Name; }

	protected:

		std::string m_Name;
	};
}

#endif //_LAYER_H_