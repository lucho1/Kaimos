#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "kspch.h"
#include "Core/Core.h"
#include "Core/Events/Event.h"

namespace Kaimos {

	struct WindowProps
	{
		std::string Title;
		uint Width, Height;

		WindowProps(const std::string& title = "Kaimos Engine", uint w = 1600, uint h = 900)
			: Title(title), Width(w), Height(h) {}
	};

	
	// -- Platform-independent Window class --
	class Window
	{
	public:

		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;
		virtual void OnUpdate() = 0;
		
		// -- Class Methods --
		virtual uint GetWidth() const = 0;
		virtual uint GetHeight() const = 0;

		virtual void* GetNativeWindow() const = 0;

		// -- Attributes --
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		// This function is implemented per-platform too (Windows window, Mac window...), each platform creates its own windows
		static ScopePtr<Window> Create(const WindowProps& props = WindowProps());
	};

	// It's basically an interface, everything is pure virtual,
	// no data at all, every window will be implemented per-platform
}

#endif