#ifndef _PLATFORM_DETECTION_H_
#define _PLATFORM_DETECTION_H_

// -- PLATFORM DETECTION - Using Predefined Macros --
// WINDOWS
#ifdef _WIN32
	// x64/x86
	#ifdef _WIN64
		#define KS_PLATFORM_WINDOWS //x64
	#else
		#error "Windows x86 is not Supported!" //x86
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

#endif //_PLATFORM_DETECTION_H_
