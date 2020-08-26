#ifndef _WINDOWSWINDOW_H_
#define _WINDOWSWINDOW_H_

#include "Core/Window.h"
#include <GLFW/glfw3.h>

namespace Kaimos {
	
	class WindowsWindow : public Window
	{
	public:

		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();
		
		// -- Layer Methods --
		void OnUpdate() override;

		// -- Class Methods --
		inline uint GetWidth() const override { return m_Data.Width; }
		inline uint GetHeight() const override { return m_Data.Height; }
		virtual void ShutdownWindow(bool terminateGLFW);

		inline void* GetNativeWindow() const override { return m_Window; }

		// -- Attributes --
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override { return m_Data.VSync; }

	private:

		virtual void Init(const WindowProps& props);
		void SetGLFWEventCallbacks() const;

	private:
		
		// Actual Window Data for GLFW callback events (so we don't pass the entire class)
		struct WindowData
		{
			EventCallbackFn EventCallback;
			uint Width = 0, Height = 0;
			std::string Title = "";
			bool VSync = true;
		};

		GLFWwindow* m_Window;
		WindowData m_Data;
	};
}

#endif