#include <lighting.hpp>

using namespace LimitlessEngine;

Lighting::Lighting(uint64_t p_count, uint64_t d_count, uint64_t s_count)
    : point_lights{p_count}, directional_lights{d_count}, spot_lights{s_count} {

}

void Lighting::update() {
    point_lights.update();
    directional_lights.update();
    spot_lights.update();
}

namespace LimitlessEngine {
    template Lighting::operator LightContainer<PointLight>&() noexcept;
    template Lighting::operator LightContainer<DirectionalLight>&() noexcept;
    template Lighting::operator LightContainer<SpotLight>&() noexcept;
}

template<typename T>
Lighting::operator LightContainer<T>&() noexcept {
    if constexpr (std::is_same_v<T, PointLight>)
        return point_lights;
    else if constexpr (std::is_same_v<T, DirectionalLight>)
        return directional_lights;
    else if constexpr (std::is_same_v<T, SpotLight>)
        return spot_lights;
    else
        static_assert(!std::is_same_v<T, T>, "No such light containter for T type");
}
