#pragma once

#include <limitless/fx/modules/module.hpp>

namespace Limitless::fx {
    template<typename Particle>
    class BeamTarget : public Module<Particle> {
    private:
        std::unique_ptr<Distribution<glm::vec3>> distribution;
    public:
        explicit BeamTarget(std::unique_ptr<Distribution<glm::vec3>> _distribution) noexcept
            : Module<Particle>(ModuleType::Beam_InitialTarget)
            , distribution {std::move(_distribution)} {}

        ~BeamTarget() override = default;

        BeamTarget(const BeamTarget& module)
            : Module<Particle>(module.type)
            , distribution {module.distribution->clone()} {}

        void initialize([[maybe_unused]] AbstractEmitter& emitter, BeamParticle& particle) noexcept override {
            particle.getTarget() = distribution->get();
        }

        [[nodiscard]] BeamTarget* clone() const override {
            return new BeamTarget(*this);
        }

        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
        [[nodiscard]] const auto& getDistribution() const noexcept { return distribution; }
    };
}