#ifndef _IMGUILAYER_H_
#define _IMGUILAYER_H_

#include "Core/Layers/Layer.h"

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/ApplicationEvent.h"

namespace Kaimos {

	class KAIMOS_API ImGuiLayer : public Layer
	{
	public:

		ImGuiLayer();
		~ImGuiLayer();

		// -- Layer Methods --
		void OnUpdate();
		void OnEvent(Event& event);
		void OnAttach();
		void OnDettach();

	private:

		// -- Class Methods --
		void SetEngineUIStyle();

		// -- Events --
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		
		bool OnWindowResizedEvent(WindowResizeEvent& e);

	private:

		float m_Time = 0.0f;
	};
}

#endif