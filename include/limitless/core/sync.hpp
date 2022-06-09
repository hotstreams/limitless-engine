#pragma once

#include <GL/glew.h>
#include <stdexcept>
#include <chrono>

namespace Limitless {
    class sync_error : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Sync {
    public:
        enum class State {
            Signaled = GL_ALREADY_SIGNALED,
            Expired = GL_TIMEOUT_EXPIRED,
            Satisfied = GL_CONDITION_SATISFIED,
            Failed = GL_WAIT_FAILED
        };
    private:
        GLsync sync {};
    public:
        Sync() = default;
        ~Sync();

        Sync(const Sync&) = delete;
        Sync& operator=(const Sync&) = delete;

        Sync(Sync&&) = default;
        Sync& operator=(Sync&&) = default;

        void place();
        void remove();

        bool isDone();
        State waitUntil(std::chrono::nanoseconds timeout);
    };
}