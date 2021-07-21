#include "kspch.h"
#include "Window.h"

#ifdef KS_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Kaimos {

	ScopePtr<Window> Window::Create(const std::string& window_name)
	{
		KS_INFO("\n\n--- CREATING APPLICATION WINDOW ---");
		#ifdef KS_PLATFORM_WINDOWS
			return CreateScopePtr<WindowsWindow>(window_name);
		#else
			KS_FATAL_ERROR("Error Creating Window: Unknown platform!");
			return nullptr;
		#endif
	}

	ScopePtr<Window> Window::Create(const WindowProps& window_props)
	{
		KS_INFO("\n\n--- CREATING APPLICATION WINDOW ---");
		#ifdef KS_PLATFORM_WINDOWS
			return CreateScopePtr<WindowsWindow>(window_props);
		#else
			KS_FATAL_ERROR("Error Creating Window: Unknown platform!");
			return nullptr;
		#endif
	}
}
