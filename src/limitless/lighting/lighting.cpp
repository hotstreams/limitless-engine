#include <limitless/lighting/lighting.hpp>

#include <limitless/core/context.hpp>
#include <limitless/core/buffer_builder.hpp>

using namespace Limitless;

namespace {
    struct SceneLighting {
        DirectionalLight directional_light {};
        glm::vec4 ambient_color {};
        uint32_t point_lights_count {};
        uint32_t dir_lights_count {};
        uint32_t spot_lights_count {};
    };

    constexpr auto SCENE_LIGHTING_BUFFER_NAME = "scene_lighting";
}

void Lighting::createLightBuffer() {
    BufferBuilder builder;
    buffer = builder.setTarget(Buffer::Type::ShaderStorage)
            .setUsage(Buffer::Usage::DynamicDraw)
            .setAccess(Buffer::MutableAccess::WriteOrphaning)
            .setDataSize(sizeof(SceneLighting))
            .build(SCENE_LIGHTING_BUFFER_NAME, context);
}

Lighting::Lighting(Context& ctx)
    : context {ctx} {
    createLightBuffer();
}

void Lighting::updateLightBuffer() {
    SceneLighting light_info {
        directional_light,
        ambient_color,
        static_cast<uint32_t>(point_lights.size()),
        directional_light.direction != glm::vec4(0.0f),
        static_cast<uint32_t>(spot_lights.size())
    };
    //TODO update changed?
    buffer->mapData(&light_info, sizeof(SceneLighting));
}

void Lighting::update() {
    // maps point lights buffer
    point_lights.update();
    spot_lights.update();

    // maps global scene light buffer
    updateLightBuffer();

    // binds light buffer to the context
    // in case if there are many scenes or lighting classes
    buffer->bindBase(context.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, SCENE_LIGHTING_BUFFER_NAME));
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

namespace Limitless {
    template Lighting::operator LightContainer<PointLight>&() noexcept;
    template Lighting::operator LightContainer<SpotLight>&() noexcept;
}
