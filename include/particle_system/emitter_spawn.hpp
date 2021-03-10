#pragma once

#include <particle_system/distribution.hpp>
#include <chrono>
#include <memory>

namespace LimitlessEngine {
    struct EmitterSpawn {
        std::chrono::time_point<std::chrono::steady_clock> last_spawn;

        enum class Mode { Spray, Burst } mode {Mode::Spray};
        uint32_t max_count {100};
        float spawn_rate {1.0f};

        std::unique_ptr<Distribution<uint32_t>> burst_count;
        int loops {-1};
        uint16_t loops_done {0};

        EmitterSpawn() = default;
        friend void swap(EmitterSpawn& lhs, EmitterSpawn& rhs) noexcept;

        EmitterSpawn(const EmitterSpawn&) noexcept;
        EmitterSpawn& operator=(const EmitterSpawn&) noexcept;

        EmitterSpawn(EmitterSpawn&&) noexcept = default;
        EmitterSpawn& operator=(EmitterSpawn&&) noexcept = default;
    };

    void swap(EmitterSpawn& lhs, EmitterSpawn& rhs) noexcept;
}
