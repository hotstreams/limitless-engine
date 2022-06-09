#include <limitless/core/sync.hpp>

using namespace Limitless;

Sync::~Sync() {
    remove();
}

Sync::State Sync::waitUntil(std::chrono::nanoseconds timeout) {
    const auto result = glClientWaitSync(sync, 0, timeout.count());
    return static_cast<State>(result);
}

bool Sync::isDone() {
    using namespace std::chrono_literals;

    if (const auto result = waitUntil(0ns); result == State::Signaled || result == State::Satisfied) {
        return true;
    }

    return false;
}

void Sync::place() {
    sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

void Sync::remove() {
    if (sync) {
        glDeleteSync(sync);
    }
}
