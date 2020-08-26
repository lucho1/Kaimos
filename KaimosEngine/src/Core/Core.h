#ifndef _CORE_H_
#define _CORE_H_

// --- Windows Definitions --
// If windows is the current platform (currently not other platform is available)
#ifdef KS_PLATFORM_WINDOWS
// Case in which we want to build Kaimos Engine as a dll
#if KS_DYNAMIC_LINK
	// Build this dll (the current one of Kaimos Engine project, not for other!)
	#ifdef KS_BUILD_DLL
		// When building the DLL (the engine itself), we EXPORT anything after KAIMOS_API
		#define KAIMOS_API __declspec(dllexport)
	#else
		// Otherwise, when not building the DLL (the engine itself), we IMPORT anything after KAIMOS_API
		#define KAIMOS_API __declspec(dllimport)
	#endif
#else
	#define KAIMOS_API
#endif
#else
	#error Kaimos Engine only supports Windows currently
#endif


// -- Assertions --
#if KS_ENABLE_ASSERTS
	#define KS_EDITOR_ASSERT(x, ...) { if(!x) { KS_EDITOR_CRITICAL("ASSERION FAILED: {0}", __VA_ARGS__); __debugbreak(); }} // Client/Editor Assert
	#define KS_ENGINE_ASSERT(x, ...) { if(!x) { KS_ENGINE_CRITICAL("ASSERION FAILED: {0}", __VA_ARGS__); __debugbreak(); }} // Core/Engine Assert
#else
	#define KS_EDITOR_ASSERT(x, ...)
	#define KS_ENGINE_ASSERT(x, ...)
#endif


// -- General Defines --
#define BIT(x) (1 << x)
#define KS_BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

// -- Typedefs --
typedef unsigned int uint;
typedef unsigned short ushort;

#endif //_CORE_H_