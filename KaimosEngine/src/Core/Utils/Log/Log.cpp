#include "kspch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Kaimos {

	Ref<spdlog::logger> Log::s_EngineLogger; // Core Logger
	Ref<spdlog::logger> Log::s_EditorLogger; // Client Logger

	void Log::Init()
	{
		// -- spdlog Initialization --
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(CreateRef<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(CreateRef<spdlog::sinks::basic_file_sink_mt>("../ResultOutputFiles/Kaimos.log", true)); // To output a file with all the logs

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		// -- Setup Engine Logger --
		s_EngineLogger = CreateRef<spdlog::logger>("Kaimos Engine", begin(logSinks), end(logSinks));
		
		spdlog::register_logger(s_EngineLogger);
		s_EngineLogger->set_level(spdlog::level::trace);
		s_EngineLogger->flush_on(spdlog::level::trace);

		// -- Setup Editor Logger --
		s_EditorLogger = CreateRef<spdlog::logger>("Kaimos Editor", begin(logSinks), end(logSinks));
		
		spdlog::register_logger(s_EditorLogger);
		s_EditorLogger->set_level(spdlog::level::trace);
		s_EditorLogger->flush_on(spdlog::level::trace);
	}
}
