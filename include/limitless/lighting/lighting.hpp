#pragma once

#include <limitless/lighting/light_container.hpp>
#include <limitless/lighting/lights.hpp>

namespace Limitless {
    class Context;

    class Lighting final {
    private:
        std::shared_ptr<Buffer> buffer;
        Context& context;

        Light directional_light;
        LightContainer<Light> lights;

        void createLightBuffer();
        void updateLightBuffer();
    public:
        explicit Lighting(Context& ctx);

        Lighting& addLight(const Light& light) {
            lights.
        }


        // ambient lighting
        glm::vec4 ambient_color {1.0f, 1.0f, 1.0f, 1.0f};

        ~Lighting() = default;

        Lighting(const Lighting&) = delete;
        Lighting(Lighting&&) = delete;

        void update();

        template<typename T>
        explicit operator LightContainer<T>&() noexcept;
    };
}
