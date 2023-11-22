#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace Limitless {
    struct StackFrame {
        std::string module;
        std::string func;
        std::string offset;
    };

    using StackTrace = std::vector<StackFrame>;

    // Return stack trace for current stack frame.
    StackTrace getStackTrace(size_t frames_to_skip = 0, size_t max_frames = 128);
}