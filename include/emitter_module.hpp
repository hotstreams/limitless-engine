#pragma once

#include <distribution.hpp>

namespace GraphicsEngine {
    class Emitter;
    struct Particle;

    enum class EmitterModuleType {
        InitialLocation,
        InitialRotation,
    };

    class EmitterModule {
    private:

    public:
        virtual void initialize(Emitter& emitter, Particle& particle) = 0;
        virtual void update(Emitter& emitter, std::vector<Particle>& particles, float dt) = 0;

        [[nodiscard]] virtual EmitterModule* clone() const noexcept = 0;
    };
}
