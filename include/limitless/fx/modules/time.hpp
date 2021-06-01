#pragma once

#include <limitless/fx/modules/module.hpp>

namespace Limitless::fx {
    template<typename Particle>
    class Time : public Module<Particle> {
    public:
        Time() noexcept
            : Module<Particle>(ModuleType::Time) {}

        ~Time() override = default;

        Time(const Time& module) = default;

        void initialize([[maybe_unused]] AbstractEmitter& emitter, Particle& particle, [[maybe_unused]] size_t index) noexcept override {
            particle.getTime() = 0.0f;
        }

        void update([[maybe_unused]] AbstractEmitter& emitter, std::vector<Particle>& particles, float dt, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Camera& camera) noexcept override {
            for (auto& particle : particles) {
                particle.getTime() += dt;
            }
        }

        [[nodiscard]] Time* clone() const override {
            return new Time(*this);
        }
    };
}