#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "Core/Core.h"
#include "Core/Events/Event.h"

#include <sstream>

namespace Kaimos {

	struct WindowProps
	{
		std::string Title = "Unnamed";
		uint Width = 960, Height = 540;

		WindowProps(const std::string& title = "Kaimos Engine", uint w = 1600, uint h = 900)
			: Title(title), Width(w), Height(h) {}
	};

	
	// -- Platform-independent Window class --
	// It's basically an interface, everything is pure virtual,
	// no data at all, every window will be implemented per-platform
	class Window
	{
	public:

		using EventCallbackFn = std::function<void(Event&)>;

		// --- Public Class Methods ---
		virtual ~Window() = default;
		virtual void OnUpdate() = 0;
		
		// --- Getters ---
		virtual uint GetWidth()			const = 0;
		virtual uint GetHeight()		const = 0;
		virtual void* GetNativeWindow()	const = 0;

		// --- Setters ---
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		// This function is implemented per-platform too (Windows window, Mac window...), each platform creates its own windows
		static ScopePtr<Window> Create(const WindowProps& props = WindowProps());

	public:

		static float s_ScreenDPIScaleFactor;
	};
}

#endif //_WINDOW_H_
