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

		static void Init();

		inline static Ref<spdlog::logger>& GetEngineLogger() { return s_EngineLogger; }
		inline static Ref<spdlog::logger>& GetEditorLogger() { return s_EditorLogger; }

	private:

		static Ref<spdlog::logger> s_EngineLogger; // Core Logger
		static Ref<spdlog::logger> s_EditorLogger; // Client Logger
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



// --- Engine/Core Logging Macros ---
#define KS_ENGINE_TRACE(...)	::Kaimos::Log::GetEngineLogger()->trace(__VA_ARGS__)
#define KS_ENGINE_INFO(...)		::Kaimos::Log::GetEngineLogger()->info(__VA_ARGS__)
#define KS_ENGINE_WARN(...)		::Kaimos::Log::GetEngineLogger()->warn(__VA_ARGS__)
#define KS_ENGINE_ERROR(...)	::Kaimos::Log::GetEngineLogger()->error(__VA_ARGS__)
#define KS_ENGINE_CRITICAL(...)	::Kaimos::Log::GetEngineLogger()->critical(__VA_ARGS__)

// --- Editor/Client Logging Macros ---
#define KS_EDITOR_TRACE(...)	::Kaimos::Log::GetEditorLogger()->trace(__VA_ARGS__)
#define KS_EDITOR_INFO(...)		::Kaimos::Log::GetEditorLogger()->info(__VA_ARGS__)
#define KS_EDITOR_WARN(...)		::Kaimos::Log::GetEditorLogger()->warn(__VA_ARGS__)
#define KS_EDITOR_ERROR(...)	::Kaimos::Log::GetEditorLogger()->error(__VA_ARGS__)
#define KS_EDITOR_CRITICAL(...)	::Kaimos::Log::GetEditorLogger()->critical(__VA_ARGS__)

#endif //_LOG_H_