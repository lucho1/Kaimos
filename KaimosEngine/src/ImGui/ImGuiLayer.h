#ifndef _IMGUILAYER_H_
#define _IMGUILAYER_H_

#include "Core/Layers/Layer.h"

#include "Core/Events/KeyEvent.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Events/ApplicationEvent.h"

namespace Kaimos {

	class ImGuiLayer : public Layer
	{
	public:

		// --- Public Class Methods ---
		ImGuiLayer() : Layer("ImGui Layer") {}
		~ImGuiLayer() = default;

		// --- Public Layer Methods ---
		virtual void OnAttach()			override;
		virtual void OnDetach()			override;
		virtual void OnEvent(Event& e)	override;

		// --- Public ImGui Methods ---
		inline void SetBlockEvents(bool block_events) { m_BlockEvents = block_events; }
		void Begin() const;
		void End() const;

	private:

		// --- Private ImGui Methods ---
		void SetEngineUIStyle() const;

	private:

		bool m_BlockEvents = true;
	};
}

#endif //_IMGUILAYER_H_
