#include "kspch.h"
#include "Application.h"

// Temporary Includes (DELETE THEM!)
#include <GLFW/glfw3.h>

namespace Kaimos {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());

		// This will bind the Application::OnEvent function to SetEventCallback(), so the callback when
		// an event happens will be Application::OnEvent. The placeholder will be replaced by whatever argument
		// is passed, so OnEvent() will be called with some argument passed (now represented by this "placeholder")
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent)); // SAME: m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
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

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose)); // If there's a WindowCloseEvent (checked in Dispatch()), dispatcher will call OnWindowClose function (same than a Lambda)
		KS_ENGINE_TRACE("{0}", e);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return false;
	}

}