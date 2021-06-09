#include "kspch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Kaimos {
	
	Ref<spdlog::logger> Log::s_EngineLogger; // Core Logger
	Ref<spdlog::logger> Log::s_EditorLogger; // Client Logger
	std::vector<Log::KaimosLog> Log::m_Logs;

	void Log::Init()
	{
		// -- spdlog Initialization --
		std::vector<spdlog::sink_ptr> log_sinks;
		log_sinks.emplace_back(CreateRef<spdlog::sinks::stdout_color_sink_mt>());
		log_sinks.emplace_back(CreateRef<spdlog::sinks::basic_file_sink_mt>(INTERNAL_OUTPUTFILES_PATH+std::string("Kaimos.log"), true)); // To output a file with all the logs

		log_sinks[0]->set_pattern("%^[%T] %n: %v%$");
		log_sinks[1]->set_pattern("[%T] [%l] %n: %v");

		// -- Setup Engine Logger --
		s_EngineLogger = CreateRef<spdlog::logger>("KAIMOS ENGINE LOG", begin(log_sinks), end(log_sinks));
		
		spdlog::register_logger(s_EngineLogger);
		s_EngineLogger->set_level(spdlog::level::trace);
		s_EngineLogger->flush_on(spdlog::level::trace);

		// -- Setup Editor Logger --
		s_EditorLogger = CreateRef<spdlog::logger>("KAIMOS EDITOR LOG", begin(log_sinks), end(log_sinks));
		
		spdlog::register_logger(s_EditorLogger);
		s_EditorLogger->set_level(spdlog::level::trace);
		s_EditorLogger->flush_on(spdlog::level::trace);
	}
}
