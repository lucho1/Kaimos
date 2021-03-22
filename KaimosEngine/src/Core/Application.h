#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "Core.h"
#include "Core/Window/Window.h"

#include "ImGui/ImGuiLayer.h"
#include "Events/ApplicationEvent.h"
#include "Layers/LayerStack.h"
#include "Time/Timestep.h"


// --- Main Declaration, Defined on EntryPoint ---
int main(int argc, char** argv);


namespace Kaimos {

	class Application
	{
	public:

		// --- Public Class Methods ---
		Application(const std::string& name = "Kaimos Engine");
		virtual ~Application();

		// --- Public Application Methods ---
		void CloseApp()						{ m_Running = false; }

		// --- Layer Methods ---
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		// --- Getters ---
		inline static Application& Get()			{ return *s_Instance; }
		inline Window& GetWindow()			const	{ return *m_Window; }
		inline ImGuiLayer* GetImGuiLayer()	const	{ return m_ImGuiLayer; }

	private:

		// --- Private Application Methods ---
		void Run();

		// --- Event Private Methods ---
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		void OnEvent(Event& e);

	private:

		// --- Application ---
		static Application* s_Instance; // Singleton of Application (we only want 1 Application)
		friend int ::main(int argc, char** argv);

		ScopePtr<Window> m_Window; // Having a unique_ptr means we don't have to worry about deleting the Window ourselves on app termination :D
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;
		
		// --- App Properties ---
		bool m_Running = true;
		bool m_Minimized = false;

		// --- Delta Time ---
		Timestep m_Timestep;
		float m_LastFrameTime = 0.0f;
	};



	// --- To be defined in Client (Editor) ---
	Application* CreateApplication();
}

#endif //_APPLICATION_H_
