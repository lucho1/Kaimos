#include "kspch.h"
#include "Application.h"

// Temporary Includes (DELETE THEM!)
#include "Events/ApplicationEvent.h"
#include <GLFW/glfw3.h>

namespace Kaimos {

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		// -- Event Test --
		WindowResizeEvent e(1080, 720);
		if (e.IsInCategory(EVENT_CATEGORY_APPLICATION))
			KS_EDITOR_TRACE(e);
		else if (e.IsInCategory(EVENT_CATEGORY_INPUT))
			KS_EDITOR_TRACE(e);

		// -- Pink Window Test --
		while (m_Running)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}
	}

}