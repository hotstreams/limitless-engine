#pragma once

#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Log.h>
#include <plog/Init.h>

#include <glm/glm.hpp>

namespace Limitless {
    class Log {
    private:
        static inline plog::RollingFileAppender<plog::TxtFormatter> file_log {"log.txt", 1024 * 1024, 0};
        static inline plog::ConsoleAppender<plog::TxtFormatter> console_log;
    public:
        static void init();
    };
}

namespace plog {
    Record& operator<<(Record& stream, const glm::vec4& v);
    Record& operator<<(Record& stream, const glm::vec3& v);
    Record& operator<<(Record& stream, const glm::vec2& v);
    Record& operator<<(Record& stream, const glm::uvec2& v);
    Record& operator<<(Record& stream, const glm::mat4& m);
}