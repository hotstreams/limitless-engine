#include <particle_system/emitter_spawn.hpp>

using namespace GraphicsEngine;

void GraphicsEngine::swap(EmitterSpawn& lhs, EmitterSpawn& rhs) noexcept {
    using std::swap;

    swap(lhs.mode, rhs.mode);
    swap(lhs.last_spawn, rhs.last_spawn);
    swap(lhs.max_count, rhs.max_count);
    swap(lhs.spawn_rate, rhs.spawn_rate);
    swap(lhs.burst_count, rhs.burst_count);
    swap(lhs.loops, rhs.loops);
    swap(lhs.loops_done, rhs.loops_done);
}

EmitterSpawn::EmitterSpawn(const EmitterSpawn& emitter) noexcept
        : last_spawn{emitter.last_spawn}, mode{emitter.mode},
          max_count{emitter.max_count}, spawn_rate{emitter.spawn_rate},
          burst_count{emitter.burst_count ? emitter.burst_count->clone() : nullptr}, loops{emitter.loops},
          loops_done{emitter.loops_done} {

}

EmitterSpawn& EmitterSpawn::operator=(const EmitterSpawn& spawn) noexcept {
    auto copied = EmitterSpawn{spawn};
    swap(*this, copied);
    return *this;
}
