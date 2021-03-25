#include "kspch.h"
#include "Application.h"

// Temporary Includes (DELETE THEM!)
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include <GLFW/glfw3.h>

namespace Kaimos {

	Application* Application::s_Instance = nullptr;

	// ----------------------- Public Class Methods -------------------------------------------------------
	Application::Application(const std::string& name)
	{
		KS_PROFILE_FUNCTION();
		KS_ENGINE_ASSERT(!s_Instance, "There already exist one instance of Application!!");
		s_Instance = this;

		m_Window = Window::Create(WindowProps(name));
		
		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer(); // It will be deleted with all the other layers in the ~LayerStack()
		PushOverlay(m_ImGuiLayer);

		// This will bind the Application::OnEvent function to SetEventCallback(), so the callback when
		// an event happens will be Application::OnEvent. The placeholder will be replaced by whatever argument
		// is passed, so OnEvent() will be called with some argument passed (now represented by this "placeholder")
		m_Window->SetEventCallback(KS_BIND_EVENT_FN(Application::OnEvent)); // SAME: m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	}

	Application::~Application()
	{
		KS_PROFILE_FUNCTION();
		Renderer::Shutdown();
	}



	// ----------------------- Layer Methods --------------------------------------------------------------
	void Application::PushLayer(Layer* layer)
	{
		KS_PROFILE_FUNCTION();
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		KS_PROFILE_FUNCTION();
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}


	
	// ----------------------- Private Application Methods ------------------------------------------------
	void Application::Run()
	{
		KS_PROFILE_FUNCTION();

		// -- Events Test --
		//WindowResizeEvent e(1080, 720);
		//if (e.IsInCategory(EVENT_CATEGORY_APPLICATION))
		//	KS_EDITOR_TRACE(e);
		//else if (e.IsInCategory(EVENT_CATEGORY_INPUT))
		//	KS_EDITOR_TRACE(e);

		while (m_Running)
		{
			KS_PROFILE_SCOPE("Run Loop");

			// -- Delta Time --
			float time = (float)glfwGetTime();			//QueryPerformanceFrequency(); QueryPerformanceCounter(); // Platform::GetTime() !!!!!!
			Timestep timestep = time - m_LastFrameTime;	// How long this frame is (dt, current time vs last frame time)
			m_LastFrameTime = time;

			// -- Layers Update --
			if (!m_Minimized)
			{
				KS_PROFILE_SCOPE("LayerStack Update");
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			// -- UI & Rendering --
			// ImGui is still updating when minimized because we don't know its behaviour on minimizing
			{
				KS_PROFILE_SCOPE("Engine UI Rendering");
				m_ImGuiLayer->Begin();

				for (Layer* layer : m_LayerStack)
					layer->OnUIRender();

				m_ImGuiLayer->End();
			}

			// -- Input Test --
			//auto [x, y] = Input::GetMousePos();
			//KS_ENGINE_TRACE(" {0}, {1}", x, y);

			// -- Window & Input Update --
			{
				KS_PROFILE_SCOPE("Window & Input Update");
				Input::OnUpdate();
				m_Window->OnUpdate();
			}
		}
	}


	
	// ----------------------- Event Private Methods ------------------------------------------------------
	void Application::OnEvent(Event& e)
	{
		KS_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(KS_BIND_EVENT_FN(Application::OnWindowClose)); // If there's a WindowCloseEvent (checked in Dispatch()), dispatcher will call OnWindowClose function (same than a Lambda)
		dispatcher.Dispatch<WindowResizeEvent>(KS_BIND_EVENT_FN(Application::OnWindowResize));
		//KS_ENGINE_TRACE("{0}", e);

		// -- Layers Events Handling --
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.IsHandled())
				break;
			
			(*it)->OnEvent(e);
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		KS_PROFILE_FUNCTION();
		uint w = e.GetWidth(), h = e.GetHeight();

		if (w == 0 || h == 0)
		{
			m_Minimized = true;
			return false;
		}

		Renderer::OnWindowResize(w, h);
		m_Minimized = false;
		return false;
	}
}