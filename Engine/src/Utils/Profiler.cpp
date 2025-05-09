#include "Utils/Profiler.h"
#include <csignal>

InstrumentationTimer* ManualInstrumentationScope::s_CurrentTimer = nullptr;

// Signal handler for graceful shutdown
static void ProfilerSignalHandler(int signal)
{
    // Ensure profiling session is properly closed before program termination
    Instrumentor::Get().EndSession();

    // Re-raise the signal after cleanup
    std::signal(signal, SIG_DFL);
    std::raise(signal);
}

// Class to register signal handlers at program startup
class ProfilerSignalHandlerRegistration
{
public:
    ProfilerSignalHandlerRegistration()
    {
        // Register handlers for common termination signals
        std::signal(SIGINT, ProfilerSignalHandler);
        std::signal(SIGTERM, ProfilerSignalHandler);
        std::signal(SIGABRT, ProfilerSignalHandler);

        // On Windows, you might also want to handle SIGBREAK
#ifdef _WIN32
        std::signal(SIGBREAK, ProfilerSignalHandler);
#endif
    }
};

// Static instance to ensure handlers are registered at program startup
static ProfilerSignalHandlerRegistration g_ProfilerSignalHandlerRegistration;