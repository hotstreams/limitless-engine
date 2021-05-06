#pragma once

#include <limitless/lighting/light_container.hpp>
#include <limitless/lighting/lights.hpp>

namespace Limitless {
    class Context;

    class Lighting final {
    private:
        std::shared_ptr<Buffer> buffer;
        Context& context;

        void createLightBuffer();
        void updateLightBuffer();
    public:
        // ambient lighting
        glm::vec4 ambient_color {1.0f, 1.0f, 1.0f, 0.5f};

        // global lighting
        DirectionalLight directional_light;

        // point lighting
        LightContainer<PointLight> point_lights;

        explicit Lighting(Context& ctx);
        ~Lighting() = default;

        Lighting(const Lighting&) = delete;
        Lighting(Lighting&&) = delete;

        void update();

        template<typename T>
        explicit operator LightContainer<T>&() noexcept;
    };
}
