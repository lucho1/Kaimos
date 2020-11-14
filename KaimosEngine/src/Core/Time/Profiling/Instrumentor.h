//
// Basic instrumentation profiler by Cherno (https://github.com/TheCherno)

// Usage: include this header file somewhere in your code (eg. precompiled header), and then use like:
//
// Instrumentor::Get().BeginSession("Session Name");        // Begin session 
// {
//     InstrumentationTimer timer("Profiled Scope Name");   // Place code like this in scopes you'd like to include in profiling
//     // Code
// }
// Instrumentor::Get().EndSession();                        // End Session
//
// You will probably want to macro-fy this, to switch on/off easily and use things like __FUNCSIG__ for the profile name.
//


//@lucho1: If we wouldn't have this header, we could be doing something like the next to numerically profile (including a profile timer & result class):
//              #define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult resRet) { m_ProfileResults.push_back(resRet); })
// This lambda returns (by reference, &) a profile result and it's pusback

//@lucho1: Only changes from original file:
//                  ·This next ifndef - define with the #endif downside
//                  ·The macros at the bottom of the document () - Extracted from the Cherno video too!
//                  · The inclusion in the Kaimos namespace

#ifndef _INSTRUMENTOR_H_
#define _INSTRUMENTOR_H_

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>

namespace Kaimos {

    struct ProfileResult
    {
        std::string Name;
        long long Start, End;
        std::thread::id ThreadID;
    };

    struct InstrumentationSession
    {
        std::string Name;
    };

    class Instrumentor
    {
    private:
        std::mutex m_Mutex;
        InstrumentationSession* m_CurrentSession;
        std::ofstream m_OutputStream;

    public:
        Instrumentor()
            : m_CurrentSession(nullptr)
        {
        }

        void BeginSession(const std::string& name, const std::string& filepath = "results.json")
        {
            std::lock_guard lock(m_Mutex);
            if (m_CurrentSession)
            {
                // If there is a current session, close it before beginning a new one
                // Subsequent profiling output for the original session will end up in the
                // newly opened session.  Better than badly formatted profiling output
                if (Log::GetEngineLogger()) // BeginSession() could be begore Log::Init()
                    KS_ENGINE_ERROR("Instrumentor::BeginSession('{0}') was called when session '{1}'  was already open", name, m_CurrentSession->Name);

                InternalEndSession();
            }

            m_OutputStream.open(filepath);
            if (m_OutputStream.is_open())
            {
                m_CurrentSession = new InstrumentationSession({ name });
                WriteHeader();
            }
            else if (Log::GetEngineLogger()) //BeginSession() could be before Log::Init()
                KS_ENGINE_ERROR("Instrumentor couldn't open results file at '{0}'", filepath);
        }

        void EndSession()
        {
            std::lock_guard lock(m_Mutex);
            InternalEndSession();
        }

        // TODO: For thread-safety, add a mutex in this function
        void WriteProfile(const ProfileResult& result)
        {
            std::stringstream json;
            std::string name = result.Name;
            std::replace(name.begin(), name.end(), '"', '\'');

            json << ",{";
            json << "\"cat\":\"function\",";
            json << "\"dur\":" << (result.End - result.Start) << ',';
            json << "\"name\":\"" << name << "\",";
            json << "\"ph\":\"X\",";
            json << "\"pid\":0,";
            json << "\"tid\":" << result.ThreadID << ",";
            json << "\"ts\":" << result.Start;
            json << "}";

            std::lock_guard lock(m_Mutex);
            if (m_CurrentSession)
            {
                m_OutputStream << json.str();
                m_OutputStream.flush();
            }
        }

        static Instrumentor& Get()
        {
            static Instrumentor instance;
            return instance;
        }

    private:

        void WriteHeader()
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
            m_OutputStream.flush();
        }

        void WriteFooter()
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();
        }

        void InternalEndSession()
        {
            if (m_CurrentSession)
            {
                WriteFooter();
                m_OutputStream.close();
                delete m_CurrentSession;
                m_CurrentSession = nullptr;
            }
        }

    };

    class InstrumentationTimer
    {
    public:

        InstrumentationTimer(const char* name)
            : m_Name(name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::high_resolution_clock::now();
        }

        ~InstrumentationTimer()
        {
            if (!m_Stopped)
                Stop();
        }

        void Stop()
        {
            auto endTimepoint = std::chrono::high_resolution_clock::now();

            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
            long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

            Instrumentor::Get().WriteProfile({ m_Name, start, end, std::this_thread::get_id() });
            m_Stopped = true;
        }

    private:

        const char* m_Name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
        bool m_Stopped;
    };

}


#if KS_ACTIVATE_PROFILE

    // Resolve which function signature macro will be used. Note that this only
    // is resolved when the (pre)compiler starts, so the syntax highlighting
    // could mark the wrong one in your editor!
    #if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
        #define HZ_FUNC_SIG __PRETTY_FUNCTION__
    #elif defined(__DMC__) && (__DMC__ >= 0x810)
        #define HZ_FUNC_SIG __PRETTY_FUNCTION__
    #elif defined(__FUNCSIG__)
        #define HZ_FUNC_SIG __FUNCSIG__
    #elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
        #define HZ_FUNC_SIG __FUNCTION__
    #elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
        #define HZ_FUNC_SIG __FUNC__
    #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
        #define HZ_FUNC_SIG __func__
    #elif defined(__cplusplus) && (__cplusplus >= 201103)
        #define HZ_FUNC_SIG __func__
    #else
        #define HZ_FUNC_SIG "HZ_FUNC_SIG unknown!"
    #endif
    
    // --- PROFILING MACROS ---
    #define KS_PROFILE_BEGIN_SESSION(name, filepath)::Kaimos::Instrumentor::Get().BeginSession(name, filepath)
    #define KS_PROFILE_END_SESSION()                ::Kaimos::Instrumentor::Get().EndSession()
    // This creates a 'Timer timer', and appending a ##line, creates a 'Timer timerline' so it doesn't crashes if called twice in a row
    #define KS_PROFILE_SCOPE(name)                  ::Kaimos::InstrumentationTimer timer##__LINE__(name)
    // _FUNCSIG_ is the complete function signature (better than _FUNCTION_ cause we know parameters inside, for the case of overloading)
    #define KS_PROFILE_FUNCTION()                   KS_PROFILE_SCOPE(HZ_FUNC_SIG)
#else
    #define KS_PROFILE_BEGIN_SESSION(name, filepath)
    #define KS_PROFILE_END_SESSION()
    #define KS_PROFILE_SCOPE(name)
    #define KS_PROFILE_FUNCTION()
#endif


#endif // _INSTRUMENTOR_H_