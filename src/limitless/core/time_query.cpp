#include <limitless/core/time_query.hpp>

using namespace Limitless;

TimeQuery::TimeQuery() {
    glGenQueries(queries.size(), queries.data());
}

TimeQuery::~TimeQuery() {
    glDeleteQueries(queries.size(), queries.data());
}

void TimeQuery::start() {
    ping_pong ? glQueryCounter(queries[0], GL_TIMESTAMP) : glQueryCounter(queries[2], GL_TIMESTAMP);
}

void TimeQuery::stop() {
    ping_pong ? glQueryCounter(queries[1], GL_TIMESTAMP) : glQueryCounter(queries[3], GL_TIMESTAMP);
}

void TimeQuery::calculate() {
    if (!initialized[ping_pong]) {
        initialized[ping_pong] = true;
        return;
    }

    GLuint64 start {}, stop {};

    if (!ping_pong) {
        GLint available;
        glGetQueryObjectiv(queries[1], GL_QUERY_RESULT_AVAILABLE, &available);
        if (available) {
            glGetQueryObjectui64v(queries[0], GL_QUERY_RESULT, &start);
            glGetQueryObjectui64v(queries[1], GL_QUERY_RESULT, &stop);
        } else {
            return; // Результат еще не готов
        }
    } else {
        GLint available;
        glGetQueryObjectiv(queries[3], GL_QUERY_RESULT_AVAILABLE, &available);
        if (available) {
            glGetQueryObjectui64v(queries[2], GL_QUERY_RESULT, &start);
            glGetQueryObjectui64v(queries[3], GL_QUERY_RESULT, &stop);
        } else {
            return; // Результат еще не готов
        }
    }

    duration = (stop - start) / 1e6f;

    ping_pong = !ping_pong;
}
