#ifndef _LAYER_H_
#define _LAYER_H_

#include "kspch.h"
#include "Core/Core.h"
#include "Events/Event.h"

namespace Kaimos {

	class Layer
	{
	public:

		Layer(const std::string& name = "Layer") : m_Name(name) {}
		virtual ~Layer() {}

		virtual void OnAttach() {} // Added to layer stack
		virtual void OnDetach() {} // Removed from layer stack
		virtual void OnUpdate() {}
		virtual void OnUIRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_Name; }

	protected:

		std::string m_Name;
	};

}
#endif