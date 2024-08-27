#pragma once

#include <limitless/fx/modules/module.hpp>

namespace Limitless::fx {
    template<typename Particle>
    class BeamSpeed : public Module<Particle> {
    private:
        // full length in %distribution seconds
        std::unique_ptr<Distribution<float>> distribution;
    public:
        BeamSpeed(std::unique_ptr<Distribution<float>> _distribution) noexcept
            : Module<Particle>(ModuleType::BeamSpeed)
            , distribution {std::move(_distribution)} {}

        ~BeamSpeed() override = default;

        auto& getDistribution() noexcept { return distribution; }
        const auto& getDistribution() const noexcept { return distribution; }

        BeamSpeed(const BeamSpeed& module) : Module<Particle>(module.type), distribution {module.distribution->clone()} {}

        void initialize([[maybe_unused]] AbstractEmitter& emitter, Particle& particle, [[maybe_unused]] size_t index) noexcept override {
            particle.speed = distribution->get();
            particle.length = 0.0f;
            particle.speed_start = std::chrono::steady_clock::now();
        }

        [[nodiscard]] BeamSpeed* clone() const override {
            return new BeamSpeed(*this);
        }

        void update([[maybe_unused]] AbstractEmitter &emitter, std::vector<Particle> &particles, [[maybe_unused]] float dt, [[maybe_unused]] const Camera &camera) noexcept override {
            using namespace std::chrono;
            for (auto& particle : particles) {
                const auto current_time = steady_clock::now();
                std::chrono::duration<double> mil = current_time - particle.speed_start;

                particle.length = mil.count() / particle.speed;
                particle.length = glm::clamp(particle.length, 0.0f, 1.0f);
            }
        }
    };
}
