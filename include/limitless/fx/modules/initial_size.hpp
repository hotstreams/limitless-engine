#pragma once

#include <limitless/fx/modules/module.hpp>

namespace Limitless::fx {
    template<typename Particle>
    class InitialSize : public Module<Particle> {
    private:
        std::unique_ptr<Distribution<float>> distribution;
    public:
        explicit InitialSize(std::unique_ptr<Distribution<float>> _distribution) noexcept
            : Module<Particle>(ModuleType::InitialSize)
            , distribution{std::move(_distribution)} {}
        ~InitialSize() override = default;

        InitialSize(const InitialSize& module)
            : Module<Particle>(module.type)
            , distribution {module.distribution->clone()} {}

        void initialize([[maybe_unused]] AbstractEmitter& emitter, Particle& particle) noexcept override {
            particle.size = distribution->get();
        }

        [[nodiscard]] InitialSize* clone() const override {
            return new InitialSize(*this);
        }

        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
        [[nodiscard]] const auto& getDistribution() const noexcept { return distribution; }
    };
}