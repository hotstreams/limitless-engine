#pragma once

#include <limitless/core/context_debug.hpp>

#include <chrono>
#include <array>

namespace Limitless {
    class TimeQuery final {
    private:
        // 0, 1 - first pair,
        // 0,3 - second pair
        // of start/stop timestamps
        std::array<GLuint, 4> queries {};
        std::array<bool, 2> initialized {};
//        std::chrono::duration<std::chrono::steady_clock> duration {};
        float duration {};
        bool ping_pong {true};
    public:
        TimeQuery();
        ~TimeQuery();

        void start();
        void stop();
        void calculate();

        [[nodiscard]] const auto& getDuration() const noexcept { return duration; }
    };
}
