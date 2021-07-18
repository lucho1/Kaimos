#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "Core/Core.h"
#include "Core/Utils/Time/Timestep.h"

#include "Events/ApplicationEvent.h"
#include "Layers/LayerStack.h"
#include "Window/Window.h"

#include "ImGui/ImGuiLayer.h"


// --- Main Declaration, Defined on EntryPoint ---
int main(int argc, char** argv);


namespace Kaimos {

	// --- Memory Usage ---
	class MemoryMetrics
	{
	public:

		uint GetAllocations()			const { return m_Allocations; }
		uint GetDeallocations()			const { return m_Deallocations; }
		uint GetAllocationsSize()		const { return m_TotalAllocated; }
		uint GetDeallocationsSize()		const { return m_TotalFreed; }

		uint GetCurrentMemoryUsage()	const { return m_TotalAllocated - m_TotalFreed; }
		uint GetCurrentAllocations()	const { return m_Allocations - m_Deallocations; }

		void AddAllocation(uint size)	const { m_TotalAllocated += size; ++m_Allocations; }
		void AddDeallocation(uint size)	const { m_TotalFreed += size; ++m_Deallocations; }

	private:

		mutable uint m_TotalAllocated = 0;
		mutable uint m_Allocations = 0;

		mutable uint m_TotalFreed = 0;
		mutable uint m_Deallocations = 0;
	};



	// --- Application ---
	class Application
	{
	public:

		// --- Public Class Methods ---
		Application(const std::string& name = "Kaimos Engine");
		virtual ~Application();

		// --- Public Application Methods ---
		void CloseApp() { m_Running = false; }
		void Serialize();
		void Deserialize();

		// --- Layer Methods ---
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		// --- Getters ---
		inline static Application& Get()		  { return *s_Instance; }
		inline float GetTime()				const { return m_Time; }
		inline Window& GetWindow()			const { return *m_Window; }
		inline ImGuiLayer* GetImGuiLayer()	const { return m_ImGuiLayer; }
		
		inline static const MemoryMetrics& GetMemoryMetrics() { return s_MemoryMetrics; }


	private:

		// --- Private Application Methods ---
		void Run();

		// --- Event Private Methods ---
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		void OnEvent(Event& e);

	private:

		// --- Memory ---
		static MemoryMetrics s_MemoryMetrics;

		// --- Application ---
		static Application* s_Instance; // Singleton of Application (we only want 1 Application)
		friend int ::main(int argc, char** argv);

		ScopePtr<Window> m_Window = nullptr; // Having a unique_ptr means we don't have to worry about deleting the Window ourselves on app termination :D
		ImGuiLayer* m_ImGuiLayer = nullptr;
		LayerStack m_LayerStack = {};
		
		// --- App Properties ---
		bool m_Running = true;
		bool m_Minimized = false;

		// --- Delta Time ---
		Timestep m_Timestep = {};
		float m_LastFrameTime = 0.0f;
		float m_Time = 0.0f;
	};
	


	// --- To be defined in Client (Editor) ---
	Application* CreateApplication();
}

#endif //_APPLICATION_H_
