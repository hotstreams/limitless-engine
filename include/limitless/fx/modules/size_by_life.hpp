#pragma once

#include <limitless/fx/modules/module.hpp>

namespace Limitless::fx {
    template<typename Particle>
    class SizeByLife : public Module<Particle> {
    private:
        std::unique_ptr<Distribution<float>> distribution;
        float factor;

        static constexpr auto MIN_SIZE = 0.0f;
        static constexpr auto MAX_SIZE = 1000.0f;
    public:
        explicit SizeByLife(std::unique_ptr<Distribution<float>> _distribution, float _factor) noexcept
            : Module<Particle>(ModuleType::SizeByLife)
            , distribution {std::move(_distribution)}
            , factor{_factor} {}

        ~SizeByLife() override = default;

        SizeByLife(const SizeByLife& module)
            : Module<Particle>(module.type)
            , distribution {module.distribution->clone()}
            , factor {module.factor} {}

        void update([[maybe_unused]] AbstractEmitter& emitter, std::vector<Particle>& particles, float dt, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Camera& camera) noexcept override {
            for (auto& particle : particles) {
                const auto tick = particle.lifetime / dt;
                const auto tick_size = glm::abs(distribution->get() - particle.size);
                particle.size += tick_size * factor / tick;
                particle.size = std::clamp(particle.size, MIN_SIZE, MAX_SIZE);
            }
        }

        [[nodiscard]] SizeByLife* clone() const override {
            return new SizeByLife(*this);
        }

        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
        [[nodiscard]] auto& getFactor() noexcept { return factor; }
    };
}