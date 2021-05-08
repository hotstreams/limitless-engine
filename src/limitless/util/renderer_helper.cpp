#include <limitless/util/renderer_helper.hpp>

#include <limitless/pipeline/render_settings.hpp>
#include <limitless/scene.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/context.hpp>

#include <limitless/ms/property.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/instances/elementary_instance.hpp>
#include <limitless/models/line.hpp>

using namespace Limitless;
using namespace Limitless::ms;

RendererHelper::RendererHelper(const RenderSettings& _settings)
    : settings {_settings} {
}

void RendererHelper::renderLightsVolume(Context& context, const Scene& scene, const Assets& assets) {
    if (scene.lighting.point_lights.empty()) {
        return;
    }

    context.enable(Capabilities::DepthTest);
    context.setDepthFunc(DepthFunc::Less);
    context.setDepthMask(DepthMask::False);
    context.disable(Capabilities::DepthTest);

    auto sphere_instance = ElementaryInstance(assets.models.at("sphere"), assets.materials.at("default"), glm::vec3(0.0f));

    context.setPolygonMode(CullFace::FrontBack, PolygonMode::Line);
    for (const auto& light : scene.lighting.point_lights) {
        sphere_instance.setPosition(light.position);
        sphere_instance.setScale(glm::vec3(light.radius));
        sphere_instance.draw(context, assets, ShaderPass::Forward, ms::Blending::Opaque);
    }
    context.setPolygonMode(CullFace::FrontBack, PolygonMode::Fill);
}

void RendererHelper::renderCoordinateSystemAxes(Context& context, const Assets& assets) {
    context.enable(Capabilities::DepthTest);
    context.setDepthFunc(DepthFunc::Less);
    context.setDepthMask(DepthMask::False);
    context.disable(Capabilities::DepthTest);

    context.setLineWidth(2.5f);

    static const auto x = std::make_shared<Line>(glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{1.0f, 0.0f, 0.0f});
    static const auto y = std::make_shared<Line>(glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});
    static const auto z = std::make_shared<Line>(glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f});

    static ElementaryInstance x_i {x, assets.materials.at("green"), {5.0f, 1.0f, 0.0f}};
    static ElementaryInstance y_i {y, assets.materials.at("blue"), {5.0f, 1.0f, 0.0f}};
    static ElementaryInstance z_i {z, assets.materials.at("red"), {5.0f, 1.0f, 0.0f}};

    x_i.draw(context, assets, ShaderPass::Forward, ms::Blending::Opaque);
    y_i.draw(context, assets, ShaderPass::Forward, ms::Blending::Opaque);
    z_i.draw(context, assets, ShaderPass::Forward, ms::Blending::Opaque);
}

void RendererHelper::renderBoundingBoxes(Context& context, const Assets& assets, const Scene& scene) {
    auto box = ElementaryInstance{assets.models.at("cube"), assets.materials.at("default"), glm::vec3{0.0f}};

    context.setLineWidth(2.5f);
    context.setPolygonMode(CullFace::FrontBack, PolygonMode::Line);
    for (const auto& [id, instance] : scene) {
        auto& bounding_box = instance->getBoundingBox();

        box.setPosition(bounding_box.center)
                .setScale(bounding_box.size);

        box.draw(context, assets, ShaderPass::Forward, ms::Blending::Opaque);
    }
    context.setPolygonMode(CullFace::FrontBack, PolygonMode::Fill);
}

void RendererHelper::render(Context& context, const Assets& assets, const Scene& scene) {
    if (settings.bounding_box) {
        renderBoundingBoxes(context, assets, scene);
    }

    if (settings.coordinate_system_axes) {
        renderCoordinateSystemAxes(context, assets);
    }

    if (settings.light_radius) {
        renderLightsVolume(context, scene, assets);
    }
}
