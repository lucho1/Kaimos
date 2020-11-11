#include "kspch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Kaimos {

	// In this case, as this is isolated and not specially handled by a Kaimos System, is an std::_ptr
	std::shared_ptr<spdlog::logger> Log::s_EngineLogger; // Core Logger
	std::shared_ptr<spdlog::logger> Log::s_EditorLogger; // Client Logger

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		
		s_EngineLogger = spdlog::stdout_color_mt("Kaimos Engine");
		s_EngineLogger->set_level(spdlog::level::trace);

		s_EditorLogger = spdlog::stdout_color_mt("Kaimos Editor");
		s_EditorLogger->set_level(spdlog::level::trace);
	}
}