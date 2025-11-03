#include <limitless/core/profiler.hpp>

namespace Limitless {

GPUProfiler::~GPUProfiler() {
    for (const auto& query : available_queries) {
        glDeleteQueries(1, &query);
    }
    for (const auto& period : pending_queries) {
        glDeleteQueries(1, &period.start_query);
        glDeleteQueries(1, &period.end_query);
    }
}

GPUProfiler::QueryID GPUProfiler::createQuery() {
    QueryID query;
    glGenQueries(1, &query);
    return query;
}

void GPUProfiler::deleteQuery(QueryID query) {
    glDeleteQueries(1, &query);
}

void GPUProfiler::ensureAvailableQueries() {
    if (available_queries.empty()) {
        // Create a new query pool (e.g., 32 queries)
        const int poolSize = 32;
        available_queries.reserve(poolSize);
        for (int i = 0; i < poolSize; ++i) {
            available_queries.push_back(createQuery());
        }
    }
}

void GPUProfiler::GPUFrame::record(Duration duration) {
    // If we've reached the maximum, remove the oldest entry
    if (durations.size() >= GPUProfiler::MAX_FRAME_COUNT) {
        auto oldest_duration = durations.front();
        
        // Update total duration by removing the oldest duration
        total_duration -= oldest_duration;
        
        // Check if we need to recalculate min/max (if the oldest was min or max)
        bool recalculate_min = (oldest_duration == min_duration);
        bool recalculate_max = (oldest_duration == max_duration);
        
        // Remove the oldest entry
        durations.erase(durations.begin());
        count--;
        
        // Recalculate min/max if necessary
        if (recalculate_min || recalculate_max) {
            min_duration = std::chrono::nanoseconds::max();
            max_duration = std::chrono::nanoseconds::zero();
            
            for (const auto& d : durations) {
                if (d < min_duration) min_duration = d;
                if (d > max_duration) max_duration = d;
            }
        }
    }
    
    // Add the new duration
    durations.push_back(duration);
    if (duration < min_duration) min_duration = duration;
    if (duration > max_duration) max_duration = duration;
    total_duration += duration;
    ++count;
}

GPUProfiler::Duration GPUProfiler::GPUFrame::getMinDuration() const noexcept {
    return min_duration;
}

GPUProfiler::Duration GPUProfiler::GPUFrame::getMaxDuration() const noexcept {
    return max_duration;
}

GPUProfiler::Duration GPUProfiler::GPUFrame::getAverageDuration() const noexcept {
    if (count == 0) return std::chrono::nanoseconds::zero();

    auto total_count = std::chrono::duration_cast<std::chrono::nanoseconds>(total_duration).count();
    auto average = total_count / count;

    return std::chrono::nanoseconds(average);
}

size_t GPUProfiler::GPUFrame::getCount() const noexcept {
    return count;
}

void GPUProfiler::checkPendingQueries() {
    for (auto it = pending_queries.begin(); it != pending_queries.end();) {
        GLint availableStart, availableEnd;
        glGetQueryObjectiv(it->start_query, GL_QUERY_RESULT_AVAILABLE, &availableStart);
        glGetQueryObjectiv(it->end_query, GL_QUERY_RESULT_AVAILABLE, &availableEnd);

        if (availableStart && availableEnd) {
            GLuint64 start, end;
            glGetQueryObjectui64v(it->start_query, GL_QUERY_RESULT, &start);
            glGetQueryObjectui64v(it->end_query, GL_QUERY_RESULT, &end);

            Duration duration = std::chrono::nanoseconds(end - start);
            frames[it->identifier].record(duration);

            // Delete used queries
            deleteQuery(it->start_query);
            deleteQuery(it->end_query);

            it = pending_queries.erase(it); // Remove from list
        } else {
            ++it; // Move to next query
        }
    }
}

GPUProfileScope::GPUProfileScope(std::string id, GPUProfiler& profiler)
    : profiler(profiler), identifier(std::move(id)) {
    profiler.ensureAvailableQueries();

    period.start_query = profiler.available_queries.back();
    profiler.available_queries.pop_back();

    period.end_query = profiler.available_queries.back();
    profiler.available_queries.pop_back();

    period.identifier = identifier;
    period.ready = false;

    glQueryCounter(period.start_query, GL_TIMESTAMP);
}

GPUProfileScope::~GPUProfileScope() {
    glQueryCounter(period.end_query, GL_TIMESTAMP);

    profiler.pending_queries.push_back(period);
}

GPUProfiler::QueryID GPUProfileScope::createTimestampQuery() {
    GPUProfiler::QueryID query;
    glGenQueries(1, &query);
    return query;
}

void GPUProfileScope::destroyTimestampQuery(GPUProfiler::QueryID query) {
    glDeleteQueries(1, &query);
}

// CPUProfiler implementation
void CPUProfiler::CPUFrame::record(Duration duration) {
    // If we've reached the maximum, remove the oldest entry
    if (durations.size() >= CPUProfiler::MAX_FRAME_COUNT) {
        auto oldest_duration = durations.front();
        
        // Update total duration by removing the oldest duration
        total_duration -= oldest_duration;
        
        // Check if we need to recalculate min/max (if the oldest was min or max)
        bool recalculate_min = (oldest_duration == min_duration);
        bool recalculate_max = (oldest_duration == max_duration);
        
        // Remove the oldest entry
        durations.erase(durations.begin());
        count--;
        
        // Recalculate min/max if necessary
        if (recalculate_min || recalculate_max) {
            min_duration = std::chrono::nanoseconds::max();
            max_duration = std::chrono::nanoseconds::zero();
            
            for (const auto& d : durations) {
                if (d < min_duration) min_duration = d;
                if (d > max_duration) max_duration = d;
            }
        }
    }
    
    // Add the new duration
    durations.push_back(duration);
    if (duration < min_duration) min_duration = duration;
    if (duration > max_duration) max_duration = duration;
    total_duration += duration;
    ++count;
}

CPUProfiler::Duration CPUProfiler::CPUFrame::getMinDuration() const noexcept {
    return min_duration;
}

CPUProfiler::Duration CPUProfiler::CPUFrame::getMaxDuration() const noexcept {
    return max_duration;
}

CPUProfiler::Duration CPUProfiler::CPUFrame::getAverageDuration() const noexcept {
    if (count == 0) return std::chrono::nanoseconds::zero();

    auto total_count = std::chrono::duration_cast<std::chrono::nanoseconds>(total_duration).count();
    auto average = total_count / count;

    return std::chrono::nanoseconds(average);
}

size_t CPUProfiler::CPUFrame::getCount() const noexcept {
    return count;
}

// CPUProfileScope implementation
CPUProfileScope::CPUProfileScope(std::string id, CPUProfiler& profiler)
    : profiler(profiler), identifier(std::move(id)) {
    start_time = std::chrono::high_resolution_clock::now();
}

CPUProfileScope::~CPUProfileScope() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<CPUProfiler::Duration>(endTime - start_time);
    profiler.frames[identifier].record(duration);
}

// Combined ProfilerScope implementation
ProfilerScope::ProfilerScope(std::string id)
    : gpu_scope(id), cpu_scope(std::move(id)) {
}

} // namespace Limitless