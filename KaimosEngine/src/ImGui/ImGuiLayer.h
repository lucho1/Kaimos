#ifndef _IMGUILAYER_H_
#define _IMGUILAYER_H_

#include "Core/Layers/Layer.h"

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/ApplicationEvent.h"

namespace Kaimos {

	class ImGuiLayer : public Layer
	{
	public:

		ImGuiLayer();
		~ImGuiLayer();

		// -- Layer Methods --
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUIRender() override;

		// -- Class Methods --
		void Begin();
		void End();

	private:

		// -- Class Methods --
		void SetEngineUIStyle() const;

	private:

		float m_Time = 0.0f;
	};
}

#endif