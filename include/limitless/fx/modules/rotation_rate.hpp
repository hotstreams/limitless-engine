#pragma once

#include <limitless/fx/modules/module.hpp>

namespace Limitless::fx {
    template<typename Particle>
    class RotationRate : public Module<Particle> {
    private:
        std::unique_ptr<Distribution<glm::vec3>> distribution;
    public:
        explicit RotationRate(std::unique_ptr<Distribution<glm::vec3>> _distribution) noexcept
            : Module<Particle>(ModuleType::RotationRate)
            , distribution {std::move(_distribution)} {}

        ~RotationRate() override = default;

        RotationRate(const RotationRate& module)
            : Module<Particle>(module.type)
            , distribution {module.distribution->clone()} {}

        void update(AbstractEmitter &emitter, std::vector<Particle> &particles, float dt, [[maybe_unused]] const Camera &camera) noexcept override {
            const auto rot = emitter.getRotation() * emitter.getLocalRotation();
            for (auto& particle : particles) {
                particle.rotation += (distribution->get() * rot) * dt;
            }
        }

        [[nodiscard]] RotationRate* clone() const noexcept override {
            return new RotationRate(*this);
        }

        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
        [[nodiscard]] const auto& getDistribution() const noexcept { return distribution; }
    };
}