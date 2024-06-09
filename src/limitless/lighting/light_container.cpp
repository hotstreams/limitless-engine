#include <limitless/lighting/light_container.hpp>

#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/lighting/light.hpp>
#include <limitless/core/context.hpp>

using namespace Limitless;

static constexpr auto SHADER_STORAGE_NAME = "LIGHTS_BUFFER";

float LightContainer::InternalLight::radiusToFalloff(float r) {
    return 1.0f / (r * r);
}

float LightContainer::InternalLight::falloffToRadius(float f) {
    return glm::sqrt(1.0f / f);
}

glm::vec2 LightContainer::InternalLight::anglesToScaleOffset(const glm::vec2& angles) {
    const auto inner = glm::radians(angles.x);
    const auto outer = glm::radians(angles.y);

    const auto inner_cos = glm::cos(inner);
    const auto outer_cos = glm::cos(outer);
    const auto scale = 1.0f / glm::max(1.0f / 1024.0f, inner_cos - outer_cos);
    const auto offset = -outer_cos * scale;

    return { scale, offset };
}

glm::vec2 LightContainer::InternalLight::scaleOffsetToAngles(const glm::vec2& scale_offset) {
    const auto outer_cos = -scale_offset.y / scale_offset.x;
    const auto inner_cos = 1.0f / (scale_offset.x + 1.0f / 1024.0f);

    const auto outer_angle = glm::degrees(glm::acos(outer_cos));
    const auto inner_angle = glm::degrees(glm::acos(inner_cos));

    return { inner_angle, outer_angle };
}

LightContainer::InternalLight::InternalLight(const Light& light) noexcept
    : color {light.getColor()}
    , position {light.getPosition(), 0.0f}
    , direction {light.getDirection(), 0.0f}
    , scale_offset {anglesToScaleOffset(light.getCone())}
    , falloff {radiusToFalloff(light.getRadius())}
    , type {static_cast<uint32_t>(light.getType())} {
}

void LightContainer::InternalLight::update(const Light& light) noexcept {
    color = light.getColor();
    position = {light.getPosition(), 0.0f};
    direction = {light.getDirection(), 0.0f};
    scale_offset = anglesToScaleOffset(light.getCone());
    falloff = radiusToFalloff(light.getRadius());
}

LightContainer::LightContainer() {
    buffer = Buffer::builder()
            .target(Buffer::Type::ShaderStorage)
            .usage(Buffer::Usage::DynamicDraw)
            .access(Buffer::MutableAccess::WriteOrphaning)
            .size(sizeof(InternalLight) * 1024)
            .build(SHADER_STORAGE_NAME, *Context::getCurrentContext());
}

Light& LightContainer::add(Light&& light) {
    // add new light to all lights
    lights.emplace(light.getId(), std::move(light));

    // add corresponding internal presentation
    internal_lights.emplace(light.getId(), light);

    return lights.at(light.getId());
}

Light& LightContainer::add(const Light& light) {
    // add new light to all lights
    auto copy = light;
    lights.emplace(light.getId(), std::move(copy));

    // add corresponding internal presentation
    internal_lights.emplace(copy.getId(), light);

    return lights.at(copy.getId());
}

void LightContainer::update() {
    // check if there were an update to lights
    bool changed {};
    for (auto& [id, light]: lights) {
        // if changed since last update
        if (light.isChanged()) {
            // update corresponding internal presentation
            internal_lights.at(id).update(light);
            changed = true;
        }
    }

    if (changed) {
        visible_lights.clear();
        visible_lights.reserve(internal_lights.size());

        for (const auto& [_, light]: internal_lights) {
            visible_lights.emplace_back(light);
        }

        buffer->mapData(visible_lights.data(), sizeof(InternalLight) * visible_lights.size());
    }

    Context::apply([this] (Context& ctx) {
        buffer->bindBase(ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, SHADER_STORAGE_NAME));
    });
}
