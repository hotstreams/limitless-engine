#pragma once

#include <chrono>
#include <limits>
#include <unordered_map>
#include <string>
#include <vector>
#include <GL/glew.h>

struct GPUProfiler {
    using QueryID = GLuint;
    using Duration = std::chrono::nanoseconds;

    struct GPUPeriod {
        QueryID startQuery;
        QueryID endQuery;
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
        Duration minDuration = std::chrono::nanoseconds::max();
        Duration maxDuration = std::chrono::nanoseconds::zero();
        Duration totalDuration = std::chrono::nanoseconds::zero();
        size_t count = 0;
    };

    std::unordered_map<std::string, GPUFrame> frames;
    std::vector<GPUPeriod> pendingQueries;

    ~GPUProfiler();

    void checkPendingQueries();
    void ensureAvailableQueries();
public:
    std::vector<QueryID> availableQueries;
    QueryID createQuery();
    void deleteQuery(QueryID query);
};

extern GPUProfiler global_gpu_profiler;

struct GPUProfileScope {
    GPUProfiler& profiler;
    const char* identifier;
    GPUProfiler::GPUPeriod period;

    GPUProfileScope(GPUProfiler& profiler, const char* id);
    ~GPUProfileScope();

private:
    static GPUProfiler::QueryID createTimestampQuery();
    static void destroyTimestampQuery(GPUProfiler::QueryID query);
};