#pragma once

#include <limitless/fx/modules/module.hpp>

namespace Limitless::fx {
    template<typename Particle>
    class InitialVelocity : public Module<Particle> {
    private:
        std::unique_ptr<Distribution<glm::vec3>> distribution;
    public:
        explicit InitialVelocity(std::unique_ptr<Distribution<glm::vec3>> _distribution) noexcept
            : Module<Particle>(ModuleType::InitialVelocity)
            , distribution {std::move(_distribution)} {}
        ~InitialVelocity() override = default;

        InitialVelocity(const InitialVelocity& module)
            : Module<Particle>(module.type)
            , distribution {module.distribution->clone()} {}

        void initialize(AbstractEmitter& emitter, Particle& particle) noexcept override {
            const auto rot = emitter.getRotation() * emitter.getLocalRotation();
            particle.velocity = distribution->get() * rot;
        }

        [[nodiscard]] InitialVelocity* clone() const override {
            return new InitialVelocity(*this);
        }

        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
    };
}