#pragma once

#include <limitless/fx/modules/distribution.hpp>
#include <optional>
#include <chrono>
#include <memory>

namespace Limitless::fx {
    class EmitterSpawn {
    public:
        enum class Mode { Spray, Burst };

        // emitting mode
        Mode mode { Mode::Spray };

        // the maximum particles count
        uint32_t max_count {100};

        // emitted particles count in one second for spray mode
        // or
        // time in seconds between bursts for burst mode
        float spawn_rate {1.0f};

        // last time emitted
        std::chrono::time_point<std::chrono::steady_clock> last_spawn;

        struct Burst {
            // particles count for 1 burst
            std::unique_ptr<Distribution<uint32_t>> burst_count;

            // desired count of loops; -1 for infinity
            int32_t loops {-1};

            // current loops count
            int32_t loops_done {0};

            Burst() = default;
            Burst(const Burst& burst)
                : burst_count { burst.burst_count ? burst.burst_count->clone() : nullptr }
                , loops {burst.loops}
                , loops_done {burst.loops_done} {}
            Burst& operator=(Burst _burst) {
                std::swap(burst_count, _burst.burst_count);
                std::swap(loops, _burst.loops);
                std::swap(loops_done, _burst.loops_done);
                return *this;
            }
            Burst(Burst&&) = default;
        };

        // for burst mode
        std::optional<Burst> burst {};

        EmitterSpawn() = default;
        EmitterSpawn(const EmitterSpawn&) = default;
        EmitterSpawn(EmitterSpawn&&) = default;

        EmitterSpawn& operator=(const EmitterSpawn&) = default;
        EmitterSpawn& operator=(EmitterSpawn&&) = default;
    };
}
