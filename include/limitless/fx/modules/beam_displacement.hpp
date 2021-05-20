#pragma once

#include <limitless/fx/modules/module.hpp>
#include <limitless/fx/modules/distribution.hpp>

namespace Limitless::fx {
    template<typename Particle>
    class BeamDisplacement : public Module<Particle> {
    private:
        std::unique_ptr<Distribution<float>> distribution;
    public:
        explicit BeamDisplacement(std::unique_ptr<Distribution<float>> _distribution) noexcept
            : Module<Particle>(ModuleType::Beam_InitialDisplacement)
            , distribution {std::move(_distribution)} {}

        ~BeamDisplacement() override = default;

        BeamDisplacement(const BeamDisplacement& module)
            : Module<Particle>(module.type)
            , distribution {module.distribution->clone()} {}

        void initialize([[maybe_unused]] AbstractEmitter& emitter, Particle& particle) noexcept override {
            particle.displacement = distribution->get();
        }

        [[nodiscard]] BeamDisplacement* clone() const override {
            return new BeamDisplacement(*this);
        }

        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
        [[nodiscard]] const auto& getDistribution() const noexcept { return distribution; }
    };
}