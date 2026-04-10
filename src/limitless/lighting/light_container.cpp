#include <limitless/lighting/light_container.hpp>

#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/lighting/light.hpp>
#include <limitless/core/context.hpp>
#include <limitless/util/frustum.hpp>
#include <limitless/util/box.hpp>

using namespace Limitless;

static constexpr auto SHADER_STORAGE_NAME = "LIGHTS_BUFFER";
static constexpr size_t DEFAULT_SSBO_CAPACITY = 1024;

static Limitless::Box makeLightBox(const Limitless::Light& light) {
    // conservative AABB of sphere influence (works for point and spot for culling purposes)
    const float r = light.getRadius();
    return { light.getPosition(), glm::vec3(r * 2.0f) };
}

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
            .size(sizeof(InternalLight) * DEFAULT_SSBO_CAPACITY)
            .build(SHADER_STORAGE_NAME, *Context::getCurrentContext());
    ssbo_capacity = DEFAULT_SSBO_CAPACITY;
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
    lights.emplace(copy.getId(), std::move(copy));

    // add corresponding internal presentation
    internal_lights.emplace(copy.getId(), light);

    return lights.at(copy.getId());
}

void LightContainer::update() {
    // check if there were an update to lights
    bool changed = false;

    auto it = lights.begin();
    while (it != lights.end()) {
        auto& [id, light] = *it;
        if (light.isRemoved()) {
            internal_lights.erase(id);
            it = lights.erase(it);
            changed = true;

        } else {
            if (light.isChanged()) {
                internal_lights.at(id).update(light);
                changed = true;
            }
            ++it;
        }
    }

    if (changed) {
        visible_lights.clear();
        visible_lights.reserve(internal_lights.size());
        visible_ids.clear();
        visible_ids.reserve(internal_lights.size());

        for (const auto& [id, light] : internal_lights) {
            visible_lights.emplace_back(light);
            visible_ids.emplace_back(id);
        }

        if (visible_lights.size() > ssbo_capacity) {
            ssbo_capacity = visible_lights.size();
            buffer->resize(sizeof(InternalLight) * ssbo_capacity);
        }

        if (!visible_lights.empty()) {
            buffer->mapData(visible_lights.data(), sizeof(InternalLight) * visible_lights.size());
        }
    }

    Context::apply([this] (Context& ctx) {
        buffer->bindBase(ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, SHADER_STORAGE_NAME));
    });
}

void LightContainer::update(const Frustum& frustum) {
    // Update internal cache from Light objects (removed/changed/hidden)
    bool internal_changed = false;

    auto it = lights.begin();
    while (it != lights.end()) {
        auto& [id, light] = *it;
        if (light.isRemoved()) {
            internal_lights.erase(id);
            it = lights.erase(it);
            internal_changed = true;
            continue;
        }

        if (light.isChanged()) {
            internal_lights.at(id).update(light);
            internal_changed = true;
            light.resetChanged();
        }

        ++it;
    }

    // Build new visible set (can change even if no light changed)
    std::vector<InternalLight> new_visible;
    std::vector<uint64_t> new_visible_ids;
    new_visible.reserve(internal_lights.size());
    new_visible_ids.reserve(internal_lights.size());

    for (const auto& [id, light] : lights) {
        if (light.isHidden()) {
            continue;
        }

        // Directional light is not stored here (Lighting routes it separately),
        // but keep type check for safety.
        if (light.isDirectional()) {
            continue;
        }

        if (!frustum.intersects(makeLightBox(light))) {
            continue;
        }

        new_visible.emplace_back(internal_lights.at(id));
        new_visible_ids.emplace_back(id);
    }

    const bool visibility_changed = (new_visible_ids != visible_ids);
    const bool should_upload = internal_changed || visibility_changed;

    if (should_upload) {
        visible_lights = std::move(new_visible);
        visible_ids = std::move(new_visible_ids);

        if (visible_lights.size() > ssbo_capacity) {
            ssbo_capacity = visible_lights.size();
            buffer->resize(sizeof(InternalLight) * ssbo_capacity);
        }

        if (!visible_lights.empty()) {
            buffer->mapData(visible_lights.data(), sizeof(InternalLight) * visible_lights.size());
        }
    }

    Context::apply([this] (Context& ctx) {
        buffer->bindBase(ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, SHADER_STORAGE_NAME));
    });
}
