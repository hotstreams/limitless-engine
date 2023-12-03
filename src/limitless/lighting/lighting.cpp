#include <limitless/lighting/lighting.hpp>

#include <limitless/core/context.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>

using namespace Limitless;

constexpr auto SCENE_LIGHTING_BUFFER_NAME = "scene_lighting";

void Lighting::createLightBuffer() {
    buffer = Buffer::builder()
            .target(Buffer::Type::ShaderStorage)
            .usage(Buffer::Usage::DynamicDraw)
            .access(Buffer::MutableAccess::WriteOrphaning)
            .size(sizeof(SceneLighting))
            .build(SCENE_LIGHTING_BUFFER_NAME, context);
}

Lighting::Lighting(Context& ctx)
    : directional_light {Light::builder().buildDefaultDirectional()}
    , context {ctx}
    , changed {true} {
    createLightBuffer();
}

const glm::vec4& Lighting::getAmbientColor() const noexcept {
    return ambient_color;
}

glm::vec4& Lighting::getAmbientColor() noexcept {
    change();
    return ambient_color;
}

void Lighting::setAmbientColor(const glm::vec4& _ambient_color) noexcept {
    ambient_color = _ambient_color;
    change();
}

const Light& Lighting::getDirectionalLight() const noexcept {
    return directional_light;
}

Light& Lighting::getDirectionalLight() noexcept {
    return directional_light;
}

void Lighting::setDirectionalLight(const Light& _directional_light) {
    directional_light = _directional_light;
    change();
}

bool Lighting::isChanged() const noexcept {
    return changed;
}

void Lighting::resetChanged() noexcept {
    changed = false;
}

void Lighting::change() noexcept {
    changed = true;
}

const std::map<uint64_t, Light>& Lighting::getLights() const noexcept {
    return punctual_lights.getLights();;
}

std::map<uint64_t, Light>& Lighting::getLights() noexcept {
    return punctual_lights.getLights();
}

LightContainer& Lighting::getLightContainer() noexcept {
    return punctual_lights;
}

Light& Lighting::add(Light&& light) {
    if (light.isDirectional()) {
        directional_light = std::move(light);
        return directional_light;
    }

    return punctual_lights.add(std::move(light));
}

Light& Lighting::add(const Light& light) {
    if (light.isDirectional()) {
        directional_light = light;
        return directional_light;
    }

    return punctual_lights.add(light);
}


void Lighting::updateSceneLightBuffer() {
    SceneLighting light_info {
        { directional_light.getDirection(), 0.0f },
        directional_light.getColor(),
        ambient_color,
        static_cast<uint32_t>(punctual_lights.visibleSize())
    };

    buffer->mapData(&light_info, sizeof(SceneLighting));
}

void Lighting::update() {
    punctual_lights.update();

    if (isChanged() || directional_light.isChanged()) {
        updateSceneLightBuffer();
        resetChanged();
        directional_light.resetChanged();
    }

    // binds light buffer to the context
    // in case if there are many scenes or lighting classes
    buffer->bindBase(context.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, SCENE_LIGHTING_BUFFER_NAME));
}
