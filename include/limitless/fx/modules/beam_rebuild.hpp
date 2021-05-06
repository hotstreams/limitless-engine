#pragma once

#include <limitless/fx/modules/module.hpp>

namespace Limitless::fx {
    template<typename Particle>
    class BeamRebuild : public Module<Particle> {
    private:
        std::unique_ptr<Distribution<float>> distribution;
    public:
        explicit BeamRebuild(std::unique_ptr<Distribution<float>> _distribution) noexcept
            : Module<Particle>(ModuleType::Beam_InitialRebuild)
            , distribution {std::move(_distribution)} {}

        ~BeamRebuild() override = default;

        BeamRebuild(const BeamRebuild& module)
            : Module<Particle>(module.type)
            , distribution {module.distribution->clone()} {}

        void initialize([[maybe_unused]] AbstractEmitter& emitter, Particle& particle) noexcept override {
            particle.rebuild_delta = std::chrono::duration<float>(distribution->get());
        }

        [[nodiscard]] BeamRebuild* clone() const override {
            return new BeamRebuild(*this);
        }

        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
    };
}