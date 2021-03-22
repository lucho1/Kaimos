#ifndef _KSPCH_H_
#define	_KSPCH_H_

#include "Core/Utils/PlatformDetection.h"

#ifdef KS_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
#endif

// --- Windows/C++ Standard Library ---
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <array>

// --- Engine Includes ---
#include "Core/Core.h"
#include "Core/Log/Log.h"
#include "Core/Time/Profiling/Instrumentor.h"


// --- Windows Include ---
#ifdef KS_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

#endif //_KSPCH_H_
