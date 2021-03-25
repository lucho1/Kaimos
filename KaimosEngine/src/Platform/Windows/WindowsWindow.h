#ifndef _WINDOWSWINDOW_H_
#define _WINDOWSWINDOW_H_

#include "Core/Application/Window/Window.h"
#include "Renderer/Foundations/GraphicsContext.h"
#include <GLFW/glfw3.h>

namespace Kaimos {
	
	class WindowsWindow : public Window
	{
	public:

		// --- Public Class Methods ---
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;
		
		// --- Public Window Methods ---
		virtual void Shutdown();

		// --- Getters ---
		inline uint GetWidth()			const override { return m_Data.Width; }
		inline uint GetHeight()			const override { return m_Data.Height; }

		inline void* GetNativeWindow()	const override { return m_Window; }

		// --- Setters ---
		void SetVSync(bool enabled)										override;
		inline bool IsVSync()											const override	{ return m_Data.VSync; }
		inline void SetEventCallback(const EventCallbackFn& callback)	override		{ m_Data.EventCallback = callback; }

	private:

		// --- Private Window Methods ---
		virtual void Init(const WindowProps& props);
		void SetGLFWEventCallbacks() const;

	private:
		
		// Window Data for GLFW callback events (so we don't pass the entire class)
		struct WindowData
		{
			EventCallbackFn EventCallback;
			uint Width = 0, Height = 0;
			std::string Title = "Unnamed";
			bool VSync = true;
		};

		ScopePtr<GraphicsContext> m_Context = nullptr;
		GLFWwindow* m_Window = nullptr;
		WindowData m_Data;
	};
}

#endif //_WINDOWSWINDOW_H_
