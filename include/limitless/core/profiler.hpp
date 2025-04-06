#pragma once

#include <chrono>
#include <limits>
#include <unordered_map>
#include <string>
#include <vector>
#include <GL/glew.h>

namespace Limitless {
    struct GPUProfiler {
        using QueryID = GLuint;
        using Duration = std::chrono::nanoseconds;
        
        // Maximum number of frames to keep per identifier
        static constexpr size_t MAX_FRAME_COUNT = 128;

        struct GPUPeriod {
            QueryID start_query;
            QueryID end_query;
            bool ready = false;
            std::string identifier;
        };

        struct GPUFrame {
            void record(Duration duration);
            Duration getMinDuration() const noexcept;
            Duration getMaxDuration() const noexcept;
            Duration getAverageDuration() const noexcept;
            size_t getCount() const noexcept;

        private:
            std::vector<Duration> durations;
            Duration min_duration = std::chrono::nanoseconds::max();
            Duration max_duration = std::chrono::nanoseconds::zero();
            Duration total_duration = std::chrono::nanoseconds::zero();
            size_t count = 0;
        };

        std::unordered_map<std::string, GPUFrame> frames;
        std::vector<GPUPeriod> pending_queries;

        ~GPUProfiler();

        void checkPendingQueries();
        void ensureAvailableQueries();
    public:
        std::vector<QueryID> available_queries;
        QueryID createQuery();
        void deleteQuery(QueryID query);
    };

    inline GPUProfiler global_gpu_profiler;

    struct GPUProfileScope {
        GPUProfiler& profiler;
        std::string identifier;
        GPUProfiler::GPUPeriod period;

        GPUProfileScope(std::string id, GPUProfiler& profiler = global_gpu_profiler);
        ~GPUProfileScope();

    private:
        static GPUProfiler::QueryID createTimestampQuery();
        static void destroyTimestampQuery(GPUProfiler::QueryID query);
    };

    struct CPUProfiler {
        using Duration = std::chrono::nanoseconds;
        using TimePoint = std::chrono::high_resolution_clock::time_point;
        
        // Maximum number of frames to keep per identifier
        static constexpr size_t MAX_FRAME_COUNT = 128;

        struct CPUFrame {
            void record(Duration duration);
            Duration getMinDuration() const noexcept;
            Duration getMaxDuration() const noexcept;
            Duration getAverageDuration() const noexcept;
            size_t getCount() const noexcept;

        private:
            std::vector<Duration> durations;
            Duration min_duration = std::chrono::nanoseconds::max();
            Duration max_duration = std::chrono::nanoseconds::zero();
            Duration total_duration = std::chrono::nanoseconds::zero();
            size_t count = 0;
        };

        std::unordered_map<std::string, CPUFrame> frames;

        ~CPUProfiler() = default;
    };

    inline CPUProfiler global_cpu_profiler;

    struct CPUProfileScope {
        CPUProfiler& profiler;
        std::string identifier;
        CPUProfiler::TimePoint start_time;

        CPUProfileScope(std::string id, CPUProfiler& profiler = global_cpu_profiler);
        ~CPUProfileScope();
    };

    struct ProfilerScope {
        GPUProfileScope gpu_scope;
        CPUProfileScope cpu_scope;

        ProfilerScope(std::string id);
        ~ProfilerScope() = default;
    };
}