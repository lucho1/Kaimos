#include "kspch.h"
#include "Application.h"

// Temporary Includes (DELETE THEM!)
#include <glad/glad.h>
#include "Input/Input.h"

namespace Kaimos {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		KS_ENGINE_ASSERT(!s_Instance, "There already exist one instance of Application!!"); // Assertion to not to have more than 1 Application instances
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_ImGuiLayer = new ImGuiLayer; // It will be deleted with all the other layers in the ~LayerStack()
		PushOverlay(m_ImGuiLayer);

		// This will bind the Application::OnEvent function to SetEventCallback(), so the callback when
		// an event happens will be Application::OnEvent. The placeholder will be replaced by whatever argument
		// is passed, so OnEvent() will be called with some argument passed (now represented by this "placeholder")
		m_Window->SetEventCallback(KS_BIND_EVENT_FN(Application::OnEvent)); // SAME: m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		
		// -- Initial vertices test --
		glGenVertexArrays(1, &vertexArr);
		glBindVertexArray(vertexArr);

		float vertices[3 * 3] = {
				-0.5f,	-0.5f,	0.0f,
				 0.5f,	-0.5f,	0.0f,
				 0.0f,	 0.5f,	0.0f
		};

		uint indices[3] = { 0, 1, 2 };

		m_VBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		m_IBuffer.reset(IndexBuffer::Create(indices, 3)); // That 3 could also be sizeof(indices)/sizeof(uint), but extra calculation!
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	}

	Application::~Application()
	{
	}

	// -- Class Methods --
	void Application::Run()
	{
		// -- Events Test --
		//WindowResizeEvent e(1080, 720);
		//if (e.IsInCategory(EVENT_CATEGORY_APPLICATION))
		//	KS_EDITOR_TRACE(e);
		//else if (e.IsInCategory(EVENT_CATEGORY_INPUT))
		//	KS_EDITOR_TRACE(e);

		while (m_Running)
		{
			// Pink Window Test
			glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// -- Initial vertices (draw) test --

			glDrawElements(GL_TRIANGLES, m_IBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

			// Layers Update
			std::vector<Layer*>::iterator it = m_LayerStack.begin();
			for (; it != m_LayerStack.end(); ++it)
				(*it)->OnUpdate();

			m_ImGuiLayer->Begin();

			it = m_LayerStack.begin();
			for (; it != m_LayerStack.end(); ++it)
				(*it)->OnUIRender();

			m_ImGuiLayer->End();

			// -- Input Test --
			//auto [x, y] = Input::GetMousePos();
			//KS_ENGINE_TRACE(" {0}, {1}", x, y);

			//for (Layer* layer : m_LayerStack)
			//	layer->OnUpdate();

			m_Window->OnUpdate();
		}
	}

	// -- Layers --
	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	// -- Events --
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(KS_BIND_EVENT_FN(Application::OnWindowClose)); // If there's a WindowCloseEvent (checked in Dispatch()), dispatcher will call OnWindowClose function (same than a Lambda)
		//KS_ENGINE_TRACE("{0}", e);

		// Layers Events handling
		//std::vector<Layer*>::iterator it = m_LayerStack.end();
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.IsHandled())
				break;
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return false;
	}
	
}