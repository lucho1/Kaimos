#ifndef _CORE_H_
#define _CORE_H_

// --- PLATFORM DETECTION ---
#include "Core/Utils/PlatformDetection.h"



// --- DYNAMIC LINKING (DLL) SUPPORT ---
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



// --- PLATFORM GLOBALS ---
#if KS_DEBUG || KS_RELEASE
	#define KS_ACTIVATE_PROFILE 1
	#if defined(KS_PLATFORM_WINDOWS)
		#define KS_DEBUGBREAK() DebugBreak();
	#elif defined(KS_PLATFORM_LINUX)
		#include <signal.h>
		#define KS_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Kaimos does't support DEBUGBREAK for this platform!"
	#endif
#else
	#define KS_ACTIVATE_PROFILE 0
	#define KS_DEBUGBREAK()
#endif



// --- ASSERTIONS ---
#if KS_ENABLE_ASSERTS
	#define KS_EDITOR_ASSERT(x, ...) { if(!(x)) { KS_EDITOR_CRITICAL("KAIMOS ASSERION: {0}", __VA_ARGS__); KS_DEBUGBREAK(); }} // Client/Editor Assert
	#define KS_ENGINE_ASSERT(x, ...) { if(!(x)) { KS_ENGINE_CRITICAL("KAIMOS ASSERION: {0}", __VA_ARGS__); KS_DEBUGBREAK(); }} // Core/Engine Assert
	#define KS_ERROR_AND_ASSERT(msg) { KS_ENGINE_ERROR(msg); KS_ENGINE_ASSERT(false, msg); }
#else
	#define KS_EDITOR_ASSERT(x, ...)
	#define KS_ENGINE_ASSERT(x, ...)
	#define KS_ERROR_AND_ASSERT(msg) { KS_ENGINE_ERROR(msg); }
#endif




// --- GENERAL DEFINES ---
// Paths
#define INTERNAL_FONTS_PATH "internal/resources/fonts/"
#define INTERNAL_ICONS_PATH "internal/resources/icons/"
#define INTERNAL_SETTINGS_PATH "internal/settings/"
#define INTERNAL_OUTPUTFILES_PATH "internal/output_files/"

// Others
#define BIT(x) (1 << x)
//#define KS_BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

// std::bind() is not the best thing to use, it is better to do a lambda, so
// the next is better than the above (https://stackoverflow.com/questions/24109737/what-are-some-uses-of-decltypeauto):
// "in generic code you want to perfectly forward a return type without knowing whether
//  you are dealing with a reference or a value. decltype(auto) gives you that ability"
// Summarizing, the next deduces the return type after the lambda is declared
#define KS_BIND_EVENT_FN(x) [this](auto&&... args)->decltype(auto) { return this->x(std::forward<decltype(args)>(args)...); }


// Data Conversors
#define BTOKB(count)	(count/1024)
#define KBTOMB(count)	(count/1024)
#define MBTOGB(count)	(count/1024)
#define BTOMB(count)	(KBTOMB(BTOKB(count)))



// --- TYPEDEFS ---
typedef unsigned int uint; // This is the same than uint32_t
typedef unsigned short ushort;

#include <memory>
namespace Kaimos {

	// There must be a line between a Kaimos Reference (for assets, mem/resource management...)
	// and an std::_ptr. We still using std::_ptr but not for things that are explicitly only for
	// Kaimos Engine, and that could, potentially and in the future, be handled by an asset manager
	// or similar, being their references handled by them
	template<typename T>
	using ScopePtr = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr ScopePtr<T> CreateScopePtr(Args&& ... args) { return std::make_unique<T>(std::forward<Args>(args)...); }
	template<typename T>
	constexpr ScopePtr<T> CreateScopePtr(T* t) { return std::unique_ptr<T>(t); }

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args) { return std::make_shared<T>(std::forward<Args>(args)...); }
	template<typename T>
	constexpr Ref<T> CreateRef(T* t) { return std::shared_ptr<T>(t); }
}

#endif //_CORE_H_
