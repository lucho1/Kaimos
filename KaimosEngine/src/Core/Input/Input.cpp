#include "kspch.h"
#include "Input.h"

#ifdef KS_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsInput.h"
#endif

namespace Kaimos
{
	ScopePtr<Input> Input::s_Instance = Input::Create();

	ScopePtr<Input> Input::Create()
	{
		#ifdef KS_PLATFORM_WINDOWS
			return CreateScopePtr<WindowsInput>();
		#else
			KS_ENGINE_ASSERT(false, "Unknown platform!");
			return nullptr;
		#endif
	}
}