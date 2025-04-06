#pragma once

#include <limitless/logging/profiler_log.hpp>
#include <cstdlib>

namespace Limitless {
    class ProfilerExitLogger {
    private:
        static bool is_registered;
        static void logOnExit() {
            ProfilerLog::logAllProfilerData();
        }
        
    public:
        // Register the exit handler
        static void registerExitLogger() {
            if (!is_registered) {
                std::atexit(logOnExit);
                is_registered = true;
                PLOG(plog::info) << "Profiler exit logger registered. Performance data will be logged on application exit.";
            }
        }
    };
    
    // Initialize static member
    bool ProfilerExitLogger::is_registered = false;
} 