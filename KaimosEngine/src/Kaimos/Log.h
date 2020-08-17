#ifndef _LOG_H_
#define _LOG_H_

#include "Core.h"
#include "spdlog/spdlog.h"

namespace Kaimos {

	class KAIMOS_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return s_EngineLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetEditorLogger() { return s_EditorLogger; }

	private:

		static std::shared_ptr<spdlog::logger> s_EngineLogger; //Core Logger
		static std::shared_ptr<spdlog::logger> s_EditorLogger; //Client Logger
	};
}

//Engine/Core Logging Macros
#define KS_ENGINE_TRACE(...)	::Kaimos::Log::GetEngineLogger()->trace(__VA_ARGS__)
#define KS_ENGINE_INFO(...)		::Kaimos::Log::GetEngineLogger()->info(__VA_ARGS__)
#define KS_ENGINE_WARN(...)		::Kaimos::Log::GetEngineLogger()->warn(__VA_ARGS__)
#define KS_ENGINE_ERROR(...)	::Kaimos::Log::GetEngineLogger()->error(__VA_ARGS__)
#define KS_ENGINE_CRITICAL(...)	::Kaimos::Log::GetEngineLogger()->critical(__VA_ARGS__)

//Editor/Client Logging Macros
#define KS_EDITOR_TRACE(...)	::Kaimos::Log::GetEditorLogger()->trace(__VA_ARGS__)
#define KS_EDITOR_INFO(...)		::Kaimos::Log::GetEditorLogger()->info(__VA_ARGS__)
#define KS_EDITOR_WARN(...)		::Kaimos::Log::GetEditorLogger()->warn(__VA_ARGS__)
#define KS_EDITOR_ERROR(...)	::Kaimos::Log::GetEditorLogger()->error(__VA_ARGS__)
#define KS_EDITOR_CRITICAL(...)	::Kaimos::Log::GetEditorLogger()->critical(__VA_ARGS__)

#endif