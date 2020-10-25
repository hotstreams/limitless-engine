#pragma once

#include <light_container.hpp>
#include <lights.hpp>

namespace GraphicsEngine {
    class Lighting {
    public:
        glm::vec4 ambient_color {1.0f, 1.0f, 1.0f, 0.5f};

        LightContainer<PointLight> point_lights;
        LightContainer<DirectionalLight> directional_lights;
        LightContainer<SpotLight> spot_lights;

        Lighting(uint64_t p_count, uint64_t d_count, uint64_t s_count);
        Lighting() = default;
        ~Lighting() = default;

        void update();

        template<typename T>
        explicit operator LightContainer<T>&() noexcept;
    };
}
