#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "Core.h"
#include "Window.h"
#include "Events/ApplicationEvent.h"

namespace Kaimos {

	class KAIMOS_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

	private:

		bool OnWindowClose(WindowCloseEvent& e);

		bool m_Running = true;
		std::unique_ptr<Window> m_Window; // Having a unique_ptr means we don't have to worry about deleting the Window ourselves on app termination :D
	};


	// To be defined in Client (Editor)
	Application* CreateApplication();
}

#endif