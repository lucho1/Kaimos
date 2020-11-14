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
        uint32_t ThreadID;
    };

    struct InstrumentationSession
    {
        std::string Name;
    };

    class Instrumentor
    {
    private:
        InstrumentationSession* m_CurrentSession;
        std::ofstream m_OutputStream;
        int m_ProfileCount;
    public:
        Instrumentor()
            : m_CurrentSession(nullptr), m_ProfileCount(0)
        {
        }

        void BeginSession(const std::string& name, const std::string& filepath = "results.json")
        {
            m_OutputStream.open(filepath);
            WriteHeader();
            m_CurrentSession = new InstrumentationSession{ name };
        }

        void EndSession()
        {
            WriteFooter();
            m_OutputStream.close();
            delete m_CurrentSession;
            m_CurrentSession = nullptr;
            m_ProfileCount = 0;
        }

        // TODO: For thread-safety, add a mutex in this function
        void WriteProfile(const ProfileResult& result)
        {
            if (m_ProfileCount++ > 0)
                m_OutputStream << ",";

            std::string name = result.Name;
            std::replace(name.begin(), name.end(), '"', '\'');

            m_OutputStream << "{";
            m_OutputStream << "\"cat\":\"function\",";
            m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
            m_OutputStream << "\"name\":\"" << name << "\",";
            m_OutputStream << "\"ph\":\"X\",";
            m_OutputStream << "\"pid\":0,";
            m_OutputStream << "\"tid\":" << result.ThreadID << ",";
            m_OutputStream << "\"ts\":" << result.Start;
            m_OutputStream << "}";

            m_OutputStream.flush();
        }

        void WriteHeader()
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
            m_OutputStream.flush();
        }

        void WriteFooter()
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();
        }

        static Instrumentor& Get()
        {
            static Instrumentor instance;
            return instance;
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

            uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
            Instrumentor::Get().WriteProfile({ m_Name, start, end, threadID });

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