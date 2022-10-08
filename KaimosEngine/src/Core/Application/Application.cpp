#include "kspch.h"
#include "Application.h"

// Temporary Includes (DELETE THEM!)
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "Core/Resources/ResourceManager.h"
#include <GLFW/glfw3.h>


// ----------------------- Memory Usage ---------------------------------------------------------------
Kaimos::MemoryMetrics Kaimos::Application::s_MemoryMetrics = {};
void* operator new(size_t size)
{
	Kaimos::Application::GetMemoryMetrics().AddAllocation((uint)size);
	return malloc(size);
}

void operator delete(void* memory, size_t size)
{
	Kaimos::Application::GetMemoryMetrics().AddDeallocation((uint)size);
	free(memory);
}

//void operator delete(void* memory)
//{
//	Kaimos::Application::GetMemoryMetrics().AddDeallocation((uint32_t)sizeof(memory));
//	free(memory);
//}
// ----------------------------------------------------------------------------------------------------



namespace Kaimos {	
	
	Application* Application::s_Instance = nullptr;


	// ----------------------- Public Class Methods -------------------------------------------------------
	Application::Application(const std::string& name)
	{
		KS_PROFILE_FUNCTION();
		KS_ENGINE_ASSERT(!s_Instance, "One instance of Application already Exists!");
		s_Instance = this;
		
		m_Window = Window::Create(name);
		//m_Window = Window::Create(WindowProps(name));

		m_ImGuiLayer = new ImGuiLayer(); // It will be deleted with all the other layers in the ~LayerStack()
		PushOverlay(m_ImGuiLayer);

		Renderer::CreateRenderer();
		Deserialize();
		Renderer::Init();

		// This will bind the Application::OnEvent function to SetEventCallback(), so the callback when
		// an event happens will be Application::OnEvent. The placeholder will be replaced by whatever argument
		// is passed, so OnEvent() will be called with some argument passed (now represented by this "placeholder")
		m_Window->SetEventCallback(KS_BIND_EVENT_FN(Application::OnEvent)); // SAME: m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	}

	Application::~Application()
	{
		KS_PROFILE_FUNCTION();
		Serialize();
		Renderer::Shutdown();
	}



	// ----------------------- Public Application Methods -------------------------------------------------
	void Application::Serialize()
	{
		KS_INFO("\n\n--- SERIALIZING KAIMOS ENGINE ---");
		Renderer::SerializeRenderer();
		Resources::ResourceManager::SerializeResources();
	}

	void Application::Deserialize()
	{
		KS_INFO("\n\n--- DESERIALIZING KAIMOS ENGINE ---");
		Renderer::DeserializeRenderer();
		Resources::ResourceManager::DeserializeResources();
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

		float last_sec_time = (float)glfwGetTime();
		uint frame_count = 0;

		while (m_Running)
		{
			KS_PROFILE_SCOPE("Run Loop");
			++frame_count;

			// -- Delta Time --
			m_Time = (float)glfwGetTime();			//QueryPerformanceFrequency(); QueryPerformanceCounter(); // Platform::GetTime() !!!!!!
			m_Timestep = m_Time - m_LastFrameTime;	// How long this frame is (dt, current time vs last frame time)
			m_LastFrameTime = m_Time;

			// -- Layers Update --
			if (!m_Minimized)
			{
				KS_PROFILE_SCOPE("LayerStack Update");
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(m_Timestep);
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

			// -- Window & Input Update --
			{
				KS_PROFILE_SCOPE("Window & Input Update");
				Input::OnUpdate();
				m_Window->OnUpdate();
			}

			// -- Debug Performance Metrics --
			float sec = m_Time - last_sec_time;

			if (sec >= 1.0f)
			{
				m_FPS = (uint)((float)frame_count / sec);
				m_LastFrameMs = 1000.0f / (float)frame_count;
				frame_count = 0;
				last_sec_time = m_Time;
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
