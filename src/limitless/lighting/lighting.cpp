#include <limitless/lighting/lighting.hpp>

#include <limitless/core/context_state.hpp>
#include <limitless/core/buffer_builder.hpp>

using namespace LimitlessEngine;

namespace {
    struct SceneLighting {
        DirectionalLight directional_light {};
        glm::vec4 ambient_color {};
        uint32_t point_lights_count {};
        uint32_t dir_lights_count {};
    };
}

Lighting::Lighting(uint64_t p_count, uint64_t s_count)
    : point_lights{p_count}, spot_lights{s_count} {
    BufferBuilder builder;
    buffer = builder.setTarget(Buffer::Type::ShaderStorage)
           .setUsage(Buffer::Usage::DynamicDraw)
           .setAccess(Buffer::MutableAccess::WriteOrphaning)
           .setDataSize(sizeof(SceneLighting))
           .build("scene_lighting", *ContextState::getState(glfwGetCurrentContext()));
}

Lighting::Lighting() {
    BufferBuilder builder;
    buffer = builder.setTarget(Buffer::Type::ShaderStorage)
            .setUsage(Buffer::Usage::DynamicDraw)
            .setAccess(Buffer::MutableAccess::WriteOrphaning)
            .setDataSize(sizeof(SceneLighting))
            .build("scene_lighting", *ContextState::getState(glfwGetCurrentContext()));}

void Lighting::updateLightBuffer() {
    SceneLighting light_info {directional_light,
                              ambient_color,
                              static_cast<uint32_t>(point_lights.size()),
                              directional_light.direction != glm::vec4{0.0f}};
    buffer->mapData(&light_info, sizeof(SceneLighting));
}

void Lighting::update() {
    point_lights.update();
    spot_lights.update();
    updateLightBuffer();
}

namespace LimitlessEngine {
    template Lighting::operator LightContainer<PointLight>&() noexcept;
    template Lighting::operator LightContainer<SpotLight>&() noexcept;
}

template<typename T>
Lighting::operator LightContainer<T>&() noexcept {
    if constexpr (std::is_same_v<T, PointLight>)
        return point_lights;
    else if constexpr (std::is_same_v<T, SpotLight>)
        return spot_lights;
    else
        static_assert(!std::is_same_v<T, T>, "No such light container for T type");
}
