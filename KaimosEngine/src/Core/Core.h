#ifndef _CORE_H_
#define _CORE_H_

// -- PLATFORM DETECTION - Using Predefined Macros --
// WINDOWS
#ifdef _WIN32
	// x64/x86
	#ifdef _WIN64
		// x64
		#define KS_PLATFORM_WINDOWS
	#else
		// x86
		#error "Windows x86 is not Supported!"
	#endif

// APPLE
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	// TARGET_OS_MAC exists on all platforms so we must check all of them (in this order)
	// to ensure that we are running on MAC and not some other Apple platform
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS Simulator not Supported!"
	#elif TARGET_OS_IPHONE == 1
		#define KS_PLATFORM_IOS
		#error "IOS is not Supported!"
	#elif TARGET_OS_MAC == 1
		#define KS_PLATFORM_MACOS
		#error "MacOS is not Currently Supported!"
	#else
		#error "Unknown Apple Platform"
	#endif

// ANDROID
// We also have to check for ANDROID before linux since it's based on linux kernel (and it has __linux__ defined)
#elif defined(__ANDROID__)
	#define KS_PLATFORM_ANDROID
	#error "Android is not Supported!"

#elif defined(__linux__)
	#define KS_PLATFORM_LINUX
	#error "Linux is not Currently Supported!"

#else
	#error "Unknown Platform!"
#endif
// -- End of PLATFORM DETECTION --


// -- DYNAMIC LINKING (DLL) SUPPORT --
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
// -- End of DLL SUPPORT --

// -- GLOBALS --
#if KS_DEBUG || KS_RELEASE
	#define KS_ACTIVATE_PROFILE 1
#else
	#define KS_ACTIVATE_PROFILE 0
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
	template<typename T, typename ... Args>
	constexpr ScopePtr<T> CreateScopePtr(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

// -- END OF GLOBALS --

#endif //_CORE_H_