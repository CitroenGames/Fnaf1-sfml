#include "Utils/Profiler.h"

#include <algorithm>
#include <csignal>
#include <functional>
#include <thread>

std::unique_ptr<InstrumentationTimer> ManualInstrumentationScope::s_CurrentTimer;

Instrumentor::Instrumentor()
    : m_CurrentSession(nullptr), m_ProfileCount(0), m_Active(false) {
}

Instrumentor::~Instrumentor() {
    if (m_Active) {
        EndSession();
    }
}

void Instrumentor::BeginSession(const std::string &name, const std::string &filepath) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    if (m_Active) {
        EndSessionUnlocked();
    }

    m_OutputStream.open(filepath);
    if (m_OutputStream.is_open()) {
        m_Active = true;
        WriteHeader();
        m_CurrentSession = std::make_unique<InstrumentationSession>(InstrumentationSession{name});
    } else {
        m_Active = false;
    }
}

void Instrumentor::EndSession() {
    std::lock_guard<std::mutex> lock(m_Mutex);
    EndSessionUnlocked();
}

void Instrumentor::EndSessionUnlocked() {
    if (!m_Active) {
        return;
    }

    WriteFooter();
    m_OutputStream.flush();
    m_OutputStream.close();
    m_CurrentSession.reset();
    m_ProfileCount = 0;
    m_Active = false;
}

void Instrumentor::WriteProfile(const ProfileResult &result) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    if (!m_Active) {
        return;
    }

    if (m_ProfileCount++ > 0) {
        m_OutputStream << ",";
    }

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

void Instrumentor::WriteHeader() {
    m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
    m_OutputStream.flush();
}

void Instrumentor::WriteFooter() {
    m_OutputStream << "]}";
    m_OutputStream.flush();
}

Instrumentor &Instrumentor::Get() {
    static Instrumentor instance;
    return instance;
}

InstrumentationTimer::InstrumentationTimer(const char *name)
    : m_Name(name), m_Stopped(false) {
    m_StartTimepoint = std::chrono::high_resolution_clock::now();
}

InstrumentationTimer::~InstrumentationTimer() {
    if (!m_Stopped) {
        Stop();
    }
}

void InstrumentationTimer::Stop() {
    auto endTimepoint = std::chrono::high_resolution_clock::now();

    long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().
            count();
    long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().
            count();

    uint32_t threadID = static_cast<uint32_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    Instrumentor::Get().WriteProfile({m_Name, start, end, threadID});

    m_Stopped = true;
}

void ManualInstrumentationScope::Begin(const char *name) {
    if (s_CurrentTimer) {
        s_CurrentTimer->Stop();
    }

    s_CurrentTimer = std::make_unique<InstrumentationTimer>(name);
}

void ManualInstrumentationScope::End() {
    if (s_CurrentTimer) {
        s_CurrentTimer->Stop();
        s_CurrentTimer.reset();
    }
}

static void ProfilerSignalHandler(int signal) {
    Instrumentor::Get().EndSession();

    std::signal(signal, SIG_DFL);
    std::raise(signal);
}

class ProfilerSignalHandlerRegistration {
public:
    ProfilerSignalHandlerRegistration() {
        std::signal(SIGINT, ProfilerSignalHandler);
        std::signal(SIGTERM, ProfilerSignalHandler);
        std::signal(SIGABRT, ProfilerSignalHandler);

#ifdef _WIN32
        std::signal(SIGBREAK, ProfilerSignalHandler);
#endif
    }
};

static ProfilerSignalHandlerRegistration g_ProfilerSignalHandlerRegistration;
