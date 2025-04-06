#include <limitless/logging/profiler_exit_logger.hpp>
#include <plog/Log.h>

namespace Limitless {
    bool ProfilerExitLogger::is_registered = false;
    
    void ProfilerExitLogger::logOnExit() {
        PLOG(plog::info) << "Application is shutting down. Logging final profiler data...";
        ProfilerLog::logAllProfilerData();
    }
} 