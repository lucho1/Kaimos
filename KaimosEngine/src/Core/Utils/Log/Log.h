#ifndef _LOG_H_
#define _LOG_H_

#include <glm/gtx/string_cast.hpp>
#include "Core/Core.h"

#pragma warning(push, 0)		// To ignore warnings related to external files or headers (which spdlog generate)
	#include <spdlog/spdlog.h>
	#include <spdlog/fmt/ostr.h>	// To pass events
#pragma warning(pop)

namespace Kaimos {

	class Log
	{
	public:

		enum class LOG_TYPES { NO_LOG = 0, TRACE_LOG, INFO_LOG, WARN_LOG, ERROR_LOG };
		struct KaimosLog
		{
			KaimosLog(const std::string& log, LOG_TYPES type) : Log(log), LogType(type) {}
			std::string Log = "";
			LOG_TYPES LogType = LOG_TYPES::NO_LOG;
		};

	public:

		static void Init();
		inline static Ref<spdlog::logger>& GetEngineLogger()				{ return s_EngineLogger; }
		inline static Ref<spdlog::logger>& GetEditorLogger()				{ return s_EditorLogger; }	

		inline static const std::vector<KaimosLog> GetLogs()				{ return m_Logs; }
		inline static void ClearLogs()										{ m_Logs.clear(); }
		inline static void AddLog(LOG_TYPES type, const std::string& log)	{ m_Logs.push_back({log, type}); }

	private:

		static Ref<spdlog::logger> s_EngineLogger;	// Core Logger
		static Ref<spdlog::logger> s_EditorLogger;	// Client Logger
		static std::vector<KaimosLog> m_Logs;		// Engine/Editor Logs for its UI
	};
}



// --- Operators Overload (<< >>) ---
template<typename Ostream, glm::length_t Length, typename T, glm::qualifier Q>
inline Ostream& operator<<(Ostream& ostream, const glm::vec<Length, T, Q>& vector)
{
	return ostream << glm::to_string(vector);
}

template<typename Ostream, glm::length_t Columns, glm::length_t Rows, typename T, glm::qualifier Q>
inline Ostream& operator<<(Ostream& ostream, const glm::mat<Columns, Rows, T, Q>& mat)
{
	return ostream << glm::to_string(mat);
}

template<typename Ostream, typename T, glm::qualifier Q>
inline Ostream& operator<<(Ostream& ostream, const glm::qua<T, Q>& quat)
{
	return ostream << glm::to_string(quat);
}



// --- __VA_ARGS__ Conversion to const char* ---
template<typename FormatString, typename... Args>
inline std::string StringFromArgs(const FormatString& fmt, const Args &... args)
{
	char arg_string[1024];
	memset(arg_string, 0, sizeof(arg_string));
	fmt::format_to(arg_string, fmt, args...);
	return std::string(arg_string);
}



// --- Engine/Core Logging Macros ---
#define KS_ENGINE_TRACE(...)	Kaimos::Log::GetEngineLogger()->trace(__VA_ARGS__)
#define KS_ENGINE_INFO(...)		Kaimos::Log::GetEngineLogger()->info(__VA_ARGS__)	
#define KS_ENGINE_WARN(...)		Kaimos::Log::GetEngineLogger()->warn(__VA_ARGS__)	
#define KS_ENGINE_ERROR(...)	Kaimos::Log::GetEngineLogger()->error(__VA_ARGS__)	
#define KS_ENGINE_CRITICAL(...)	Kaimos::Log::GetEngineLogger()->critical(__VA_ARGS__)

// --- Editor/Client Logging Macros ---
#define KS_EDITOR_TRACE(...)	Kaimos::Log::AddLog(Kaimos::Log::LOG_TYPES::TRACE_LOG, StringFromArgs(__VA_ARGS__))
#define KS_EDITOR_INFO(...)		Kaimos::Log::AddLog(Kaimos::Log::LOG_TYPES::INFO_LOG, StringFromArgs(__VA_ARGS__))
#define KS_EDITOR_WARN(...)		Kaimos::Log::AddLog(Kaimos::Log::LOG_TYPES::WARN_LOG, StringFromArgs(__VA_ARGS__))
#define KS_EDITOR_ERROR(...)	Kaimos::Log::AddLog(Kaimos::Log::LOG_TYPES::ERROR_LOG, StringFromArgs(__VA_ARGS__))
#define KS_EDITOR_CRITICAL(...)	Kaimos::Log::AddLog(Kaimos::Log::LOG_TYPES::ERROR_LOG, StringFromArgs(__VA_ARGS__))

// --- Common Logging Macros (for both Consoles) ---
#define KS_TRACE(...)			{ Kaimos::Log::GetEngineLogger()->trace(__VA_ARGS__);		Kaimos::Log::AddLog(Kaimos::Log::LOG_TYPES::TRACE_LOG, StringFromArgs(__VA_ARGS__));	}
#define KS_INFO(...)			{ Kaimos::Log::GetEngineLogger()->info(__VA_ARGS__);		Kaimos::Log::AddLog(Kaimos::Log::LOG_TYPES::INFO_LOG, StringFromArgs(__VA_ARGS__));		}
#define KS_WARN(...)			{ Kaimos::Log::GetEngineLogger()->warn(__VA_ARGS__);		Kaimos::Log::AddLog(Kaimos::Log::LOG_TYPES::WARN_LOG, StringFromArgs(__VA_ARGS__));		}
#define KS_ERROR(...)			{ Kaimos::Log::GetEngineLogger()->error(__VA_ARGS__);		Kaimos::Log::AddLog(Kaimos::Log::LOG_TYPES::ERROR_LOG, StringFromArgs(__VA_ARGS__));	}
#define KS_CRITICAL(...)		{ Kaimos::Log::GetEngineLogger()->critical(__VA_ARGS__);	Kaimos::Log::AddLog(Kaimos::Log::LOG_TYPES::ERROR_LOG, StringFromArgs(__VA_ARGS__));	}

#endif //_LOG_H_
