#include <limitless/core/profiler.hpp>

GPUProfiler::~GPUProfiler() {
    for (const auto& query : availableQueries) {
        glDeleteQueries(1, &query);
    }
    for (const auto& period : pendingQueries) {
        glDeleteQueries(1, &period.startQuery);
        glDeleteQueries(1, &period.endQuery);
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
    if (availableQueries.empty()) {
        // Создаем новый пул запросов (например, 32 запроса)
        const int poolSize = 32;
        availableQueries.reserve(poolSize);
        for (int i = 0; i < poolSize; ++i) {
            availableQueries.push_back(createQuery());
        }
    }
}

void GPUProfiler::GPUFrame::record(Duration duration) {
    durations.push_back(duration);
    if (duration < minDuration) minDuration = duration;
    if (duration > maxDuration) maxDuration = duration;
    totalDuration += duration;
    ++count;
}

GPUProfiler::Duration GPUProfiler::GPUFrame::getMinDuration() const noexcept {
    return minDuration;
}

GPUProfiler::Duration GPUProfiler::GPUFrame::getMaxDuration() const noexcept {
    return maxDuration;
}

GPUProfiler::Duration GPUProfiler::GPUFrame::getAverageDuration() const noexcept {
    if (count == 0) return std::chrono::nanoseconds::zero();

    auto total_count = std::chrono::duration_cast<std::chrono::nanoseconds>(totalDuration).count();
    auto average = total_count / count;

    return std::chrono::nanoseconds(average);
}

size_t GPUProfiler::GPUFrame::getCount() const noexcept {
    return count;
}

void GPUProfiler::checkPendingQueries() {
    for (auto it = pendingQueries.begin(); it != pendingQueries.end();) {
        GLint availableStart, availableEnd;
        glGetQueryObjectiv(it->startQuery, GL_QUERY_RESULT_AVAILABLE, &availableStart);
        glGetQueryObjectiv(it->endQuery, GL_QUERY_RESULT_AVAILABLE, &availableEnd);

        if (availableStart && availableEnd) {
            GLuint64 start, end;
            glGetQueryObjectui64v(it->startQuery, GL_QUERY_RESULT, &start);
            glGetQueryObjectui64v(it->endQuery, GL_QUERY_RESULT, &end);

            Duration duration = std::chrono::nanoseconds(end - start);
            frames[it->identifier].record(duration);

            // Удаляем использованные запросы
            deleteQuery(it->startQuery);
            deleteQuery(it->endQuery);

            it = pendingQueries.erase(it); // Удаляем из списка
        } else {
            ++it; // Переходим к следующему запросу
        }
    }
}

GPUProfileScope::GPUProfileScope(GPUProfiler& profiler, const char* id)
    : profiler(profiler), identifier(id) {
    profiler.ensureAvailableQueries();

    period.startQuery = profiler.availableQueries.back();
    profiler.availableQueries.pop_back();

    period.endQuery = profiler.availableQueries.back();
    profiler.availableQueries.pop_back();

    period.identifier = identifier;
    period.ready = false;

    glQueryCounter(period.startQuery, GL_TIMESTAMP);
}

GPUProfileScope::~GPUProfileScope() {
    glQueryCounter(period.endQuery, GL_TIMESTAMP);

    profiler.pendingQueries.push_back(period);
}

GPUProfiler::QueryID GPUProfileScope::createTimestampQuery() {
    GPUProfiler::QueryID query;
    glGenQueries(1, &query);
    return query;
}

void GPUProfileScope::destroyTimestampQuery(GPUProfiler::QueryID query) {
    glDeleteQueries(1, &query);
}

GPUProfiler global_gpu_profiler;