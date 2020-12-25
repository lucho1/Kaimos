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

		ImGuiLayer();
		~ImGuiLayer() = default;

		// -- Layer Methods --
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUIRender() override;
		virtual void OnEvent(Event& e) override;

		// -- Class Methods --
		void SetBlockEvents(bool block_events) { m_BlockEvents = block_events; }
		void Begin();
		void End();

	private:

		// -- Class Methods --
		void SetEngineUIStyle() const;

	private:

		bool m_BlockEvents = true;
	};
}

#endif