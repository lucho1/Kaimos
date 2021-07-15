#include "kspch.h"
#include "Window.h"

#ifdef KS_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Kaimos {

	ScopePtr<Window> Window::Create(const WindowProps& props)
	{
		KS_ENGINE_INFO("\n\n--- CREATING APPLICATION WINDOW ---");
		#ifdef KS_PLATFORM_WINDOWS
			return CreateScopePtr<WindowsWindow>(props);
		#else
			HZ_CORE_ASSERT(false, "Unknown platform!");
			return nullptr;
		#endif
	}
}
