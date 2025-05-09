#pragma once

#define PROFILING 1 // we will eventually put it under a config or debug config?

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
    std::mutex m_Mutex;
    bool m_Active;
public:
    Instrumentor()
        : m_CurrentSession(nullptr), m_ProfileCount(0), m_Active(false)
    {
    }

    ~Instrumentor()
    {
        if (m_Active)
            EndSession();
    }

    void BeginSession(const std::string& name, const std::string& filepath = "results.json")
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (m_Active)
        {
            // End previous session if one exists
            EndSession();
        }

        m_Active = true;
        m_OutputStream.open(filepath);
        if (m_OutputStream.is_open())
        {
            WriteHeader();
            m_CurrentSession = new InstrumentationSession{ name };
        }
        else
        {
            // Handle file open error
            m_Active = false;
        }
    }

    void EndSession()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (!m_Active)
            return;

        WriteFooter();
        m_OutputStream.flush();
        m_OutputStream.close();
        delete m_CurrentSession;
        m_CurrentSession = nullptr;
        m_ProfileCount = 0;
        m_Active = false;
    }

    void WriteProfile(const ProfileResult& result)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (!m_Active)
            return;

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

// For manually managed profiling scopes
class ManualInstrumentationScope
{
private:
    static InstrumentationTimer* s_CurrentTimer;

public:
    static void Begin(const char* name)
    {
        // End previous timer if one exists
        if (s_CurrentTimer != nullptr)
        {
            s_CurrentTimer->Stop();
            delete s_CurrentTimer;
        }

        // Create new timer
        s_CurrentTimer = new InstrumentationTimer(name);
    }

    static void End()
    {
        if (s_CurrentTimer != nullptr)
        {
            s_CurrentTimer->Stop();
            delete s_CurrentTimer;
            s_CurrentTimer = nullptr;
        }
    }
};

// Macros for easy profiling
#if PROFILING
    // Helper macro to concatenate strings in macros
#define PROFILE_CONCAT_INTERNAL(x, y) x##y
#define PROFILE_CONCAT(x, y) PROFILE_CONCAT_INTERNAL(x, y)

    // Macros for profiling
#define PROFILE_BEGIN_SESSION(name, filepath) Instrumentor::Get().BeginSession(name, filepath)
#define PROFILE_END_SESSION() Instrumentor::Get().EndSession()
#define PROFILE_SCOPE(name) InstrumentationTimer PROFILE_CONCAT(timer, __LINE__)(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)

    // Use PROFILE_FUNCTION_SIG() for more detailed function signatures
#if defined(__GNUC__) || defined(__clang__)
#define PROFILE_FUNCTION_SIG() PROFILE_SCOPE(__PRETTY_FUNCTION__)
#elif defined(_MSC_VER)
#define PROFILE_FUNCTION_SIG() PROFILE_SCOPE(__FUNCSIG__)
#else
#define PROFILE_FUNCTION_SIG() PROFILE_FUNCTION()
#endif

    // Macros for manually managed profiling
#define PROFILE_BEGIN(name) ManualInstrumentationScope::Begin(name)
#define PROFILE_END() ManualInstrumentationScope::End()

    // Register an automatic cleanup handler
#define PROFILE_AUTO_SESSION(name, filepath) \
    class ProfilerSessionGuard { \
    public: \
        ProfilerSessionGuard(const std::string& name, const std::string& filepath) { \
            Instrumentor::Get().BeginSession(name, filepath); \
        } \
        ~ProfilerSessionGuard() { \
            Instrumentor::Get().EndSession(); \
        } \
    }; \
    static ProfilerSessionGuard PROFILE_CONCAT(autoProfilerGuard, __LINE__)(name, filepath)

#else
    // No-op definitions when profiling is disabled
#define PROFILE_BEGIN_SESSION(name, filepath)
#define PROFILE_END_SESSION()
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()
#define PROFILE_FUNCTION_SIG()
#define PROFILE_BEGIN(name)
#define PROFILE_END()
#define PROFILE_AUTO_SESSION(name, filepath)
#endif