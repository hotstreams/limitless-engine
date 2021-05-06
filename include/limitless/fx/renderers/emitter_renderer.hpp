#pragma once

#include <type_traits>

namespace Limitless::fx {
    class AbstractEmitter;

    static constexpr auto EMITTER_STORAGE_INSTANCE_COUNT = 3;

    class AbstractEmitterRenderer {

    };

    template<typename Particle>
    class EmitterRenderer : public AbstractEmitterRenderer {
        static_assert(!std::is_same_v<Particle, Particle>, "not yet implemented");
    };
}
