#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "Core.h"
#include "Window.h"
#include "ImGui/ImGuiLayer.h"
#include "Events/ApplicationEvent.h"
#include "Layers/LayerStack.h"

// TEMP
#include "Renderer/Buffer.h"
#include "Renderer/OrthographicCamera.h"

namespace Kaimos {

	class Application
	{
	public:

		Application();
		virtual ~Application();

		// -- Class Methods --
		void Run();

		// -- Events --
		void OnEvent(Event& e);

		// -- Layers --
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		// -- Getters --
		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() const { return *m_Window; }

	private:

		// -- Events --
		bool OnWindowClose(WindowCloseEvent& e);

		// -- Class Variables --
		bool m_Running = true;
		std::unique_ptr<Window> m_Window; // Having a unique_ptr means we don't have to worry about deleting the Window ourselves on app termination :D
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;

		OrthographicCamera m_Camera;

		std::shared_ptr<VertexArray> m_VArray;

		static Application* s_Instance; // Singleton of Application (we only want 1)
	};


	// To be defined in Client (Editor)
	Application* CreateApplication();
}

#endif