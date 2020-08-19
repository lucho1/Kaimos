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

		void OnUpdate() override;

		inline uint GetWidth() const override { return m_Data.Width; }
		inline uint GetHeight() const override { return m_Data.Height; }

		// Attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

	private:

		virtual void Init(const WindowProps& props);
		virtual void Shutdown();

	private:
		
		// Actual Window Data for GLFW callback events (so we don't pass the entire class)
		struct WindowData
		{
			EventCallbackFn EventCallback;
			uint Width, Height;
			std::string Title;
			bool VSync;
		};

		GLFWwindow* m_Window;
		WindowData m_Data;
	};

}

#endif