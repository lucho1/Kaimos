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
typedef unsigned int uint; // This is the same than uint32_t
typedef unsigned short ushort;

#include <memory>
namespace Kaimos
{
	// There must be a line between a Kaimos Reference (for assets, mem/resource management...)
	// and an std::_ptr. We still using std::_ptr but not for things that are explicitly only for
	// Kaimos Engine, and that could, potentially and in the future, be handled by an asset manager
	// or similar, being their references handled by them
	template<typename T>
	using ScopePtr = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;
}


#endif //_CORE_H_