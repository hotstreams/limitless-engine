#pragma once

#include <limitless/fx/modules/module.hpp>
#include <array>

namespace Limitless::fx {
    template<typename Particle>
    class CustomMaterial : public Module<Particle> {
    private:
        std::array<std::unique_ptr<Distribution<float>>, 4> properties;
    public:
        CustomMaterial(std::unique_ptr<Distribution<float>> prop1,
                       std::unique_ptr<Distribution<float>> prop2,
                       std::unique_ptr<Distribution<float>> prop3,
                       std::unique_ptr<Distribution<float>> prop4 ) noexcept
            : Module<Particle>(ModuleType::CustomMaterial)
            , properties {std::move(prop1), std::move(prop2), std::move(prop3), std::move(prop4)} {}

        ~CustomMaterial() override = default;

        CustomMaterial(const CustomMaterial& module)
            : Module<Particle>(module.type){
            for (size_t i = 0; i < module.properties.size(); ++i) {
                if (module.properties[i]) {
                    properties[i] = std::unique_ptr<Distribution<float>>(module.properties[i]->clone());
                }
            }
        }

        auto& getProperties() noexcept { return properties; }
        const auto& getProperties() const noexcept { return properties; }

        void initialize([[maybe_unused]] AbstractEmitter& emitter, Particle& particle, [[maybe_unused]] size_t index) noexcept override {
            for (size_t i = 0; i < properties.size(); ++i) {
                if (properties[i]) {
                    particle.properties[i] = properties[i]->get();
                }
            }
        }

        [[nodiscard]] CustomMaterial* clone() const override {
            return new CustomMaterial(*this);
        }
    };
}