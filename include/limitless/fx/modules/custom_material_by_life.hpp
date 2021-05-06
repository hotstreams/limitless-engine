#pragma once

#include <limitless/fx/modules/module.hpp>

namespace Limitless::fx {
    template<typename Particle>
    class CustomMaterialByLife : public Module<Particle> {
    private:
        std::array<std::unique_ptr<Distribution<float>>, 4> properties;
    public:
        CustomMaterialByLife(std::unique_ptr<Distribution<float>> prop1,
                             std::unique_ptr<Distribution<float>> prop2,
                             std::unique_ptr<Distribution<float>> prop3,
                             std::unique_ptr<Distribution<float>> prop4) noexcept
            : Module<Particle>(ModuleType::CustomMaterialByLife)
            , properties {std::move(prop1), std::move(prop2), std::move(prop3), std::move(prop4)} {}

        ~CustomMaterialByLife() override = default;

        CustomMaterialByLife(const CustomMaterialByLife& module)
            : Module<Particle>(module.type) {
            for (size_t i = 0; i < module.properties.size(); ++i) {
                if (module.properties[i]) {
                    properties[i] = std::unique_ptr<Distribution<float>>(module.properties[i]->clone());
                }
            }
        }

        auto& getProperties() noexcept { return properties; }

        void update([[maybe_unused]] AbstractEmitter& emitter, std::vector<Particle>& particles, float dt, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Camera& camera) noexcept override {
            for (auto& particle : particles) {
                for (size_t i = 0; i < properties.size(); ++i) {
                    if (properties[i]) {
                        const auto tick = particle.lifetime / dt;
                        particle.properties[i] += (properties[i]->get() - particle.properties[i]) / tick;
                    }
                }
            }
        }

        [[nodiscard]] CustomMaterialByLife* clone() const override {
            return new CustomMaterialByLife(*this);
        }
    };
}