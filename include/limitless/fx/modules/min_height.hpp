#pragma once

#include <limitless/fx/modules/module.hpp>

namespace Limitless::fx {
    template<typename Particle>
    class MinHeight : public Module<Particle> {
    private:
        float min_height;
    public:
        explicit MinHeight(float _min_height) noexcept
            : Module<Particle>(ModuleType::MinHeight)
            , min_height {_min_height} {}

        ~MinHeight() override = default;

        MinHeight(const MinHeight& module)
            : Module<Particle>(module.type)
            , min_height {module.min_height} {}

        void update([[maybe_unused]] AbstractEmitter& emitter, std::vector<Particle>& particles, float dt, [[maybe_unused]] const Camera& camera) noexcept override {
            for (auto& particle : particles) {
                if (particle.position.y < min_height
                    || particle.position.y + particle.velocity.y * dt < min_height) {
                    particle.lifetime = 0.0f;
                }
            }
        }

        [[nodiscard]] MinHeight* clone() const override {
            return new MinHeight(*this);
        }

        [[nodiscard]] float getMinHeight() const noexcept { return min_height; }
        void setMinHeight(float height) noexcept { min_height = height; }
    };
}
