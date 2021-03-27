#pragma once

#include <limitless/lighting/light_container.hpp>
#include <limitless/lighting/lights.hpp>

namespace LimitlessEngine {
    class Lighting final {
    private:
        std::shared_ptr<Buffer> buffer;

        void updateLightBuffer();
    public:
        glm::vec4 ambient_color {1.0f, 1.0f, 1.0f, 0.5f};
        DirectionalLight directional_light;

        LightContainer<PointLight> point_lights;
        LightContainer<SpotLight> spot_lights;

        Lighting(uint64_t p_count, uint64_t s_count);
        Lighting();
        ~Lighting() = default;

        void update();

        template<typename T>
        explicit operator LightContainer<T>&() noexcept;
    };
}
