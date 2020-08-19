#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "kspch.h"
#include "Core.h"
#include "Events/Event.h"

namespace Kaimos {

	struct WindowProps
	{
		std::string Title;
		uint Width, Height;

		WindowProps(const std::string& title = "Kaimos Engine", uint w = 1080, uint h = 720)
			: Title(title), Width(w), Height(h) {}
	};

	
	// -- Platform-independent Window class --
	class KAIMOS_API Window
	{
	public:

		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}
		virtual void OnUpdate() = 0;
		
		virtual uint GetWidth() const = 0;
		virtual uint GetHeight() const = 0;

		// Attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		// This function is implemented per-platform too (Windows window, Mac window...), each platform creates its own windows
		static Window* Create(const WindowProps& props = WindowProps());
	};

	// It's basically an interface, everything is pure virtual,
	// no data at all, every window will be implemented per-platform
}

#endif