#include <cmath>
#include <limitless/util/renderer_helper.hpp>

#include <limitless/pipeline/render_settings.hpp>
#include <limitless/scene.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/context.hpp>

#include <limitless/ms/property.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/models/line.hpp>
#include <limitless/instances/model_instance.hpp>
#include <limitless/models/cylinder.hpp>
#include <iostream>

using namespace Limitless;
using namespace Limitless::ms;

RendererHelper::RendererHelper(const RenderSettings& _settings)
    : settings {_settings} {
}

void RendererHelper::renderLightsVolume(Context& context, const Lighting& lighting, const Assets& assets, const Camera& camera) {
    if (lighting.point_lights.empty() && lighting.spot_lights.empty()) {
        return;
    }

    context.enable(Capabilities::DepthTest);
    context.setDepthFunc(DepthFunc::Less);
    context.setDepthMask(DepthMask::False);
    context.disable(Capabilities::DepthTest);

    auto sphere_instance = ModelInstance(assets.models.at("sphere"), assets.materials.at("default"), glm::vec3(0.0f));

    context.setPolygonMode(CullFace::FrontBack, PolygonMode::Line);
    for (const auto& light : lighting.point_lights) {
        sphere_instance.setPosition(light.position);
        sphere_instance.setScale(glm::vec3(light.radius));
        sphere_instance.update(context, camera);
        sphere_instance.draw(context, assets, ShaderPass::Forward, ms::Blending::Opaque);
    }

    for (const auto& light : lighting.spot_lights) {
        auto cone = std::make_shared<Cylinder>(0.0f, light.radius * std::sin(glm::acos(light.cutoff)), light.radius);
        auto cone_instance = ModelInstance(cone, assets.materials.at("default"), glm::vec3(0.0f));

        cone_instance.setPosition(light.position);

        auto y = glm::vec3{0.0f, 1.0f, 0.0f};
        auto a = glm::cross(y, glm::vec3{light.direction});
        auto angle = glm::acos(glm::dot(y, glm::vec3{light.direction}));

        cone_instance.setRotation(a * angle);
        cone_instance.update(context, camera);
        cone_instance.draw(context, assets, ShaderPass::Forward, ms::Blending::Opaque);
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

    static ModelInstance x_i {x, assets.materials.at("green"), {5.0f, 1.0f, 0.0f}};
    static ModelInstance y_i {y, assets.materials.at("blue"), {5.0f, 1.0f, 0.0f}};
    static ModelInstance z_i {z, assets.materials.at("red"), {5.0f, 1.0f, 0.0f}};

    x_i.draw(context, assets, ShaderPass::Forward, ms::Blending::Opaque);
    y_i.draw(context, assets, ShaderPass::Forward, ms::Blending::Opaque);
    z_i.draw(context, assets, ShaderPass::Forward, ms::Blending::Opaque);
}

void RendererHelper::renderBoundingBoxes(Context& context, const Assets& assets, Instances& instances) {
    auto box = ModelInstance{assets.models.at("cube"), assets.materials.at("default"), glm::vec3{0.0f}};

    context.setLineWidth(2.5f);
    context.setPolygonMode(CullFace::FrontBack, PolygonMode::Line);
    for (const auto& instance : instances) {
        auto& bounding_box = instance.get().getBoundingBox();

        box.setPosition(bounding_box.center).setScale(bounding_box.size);

        box.draw(context, assets, ShaderPass::Forward, ms::Blending::Opaque);
    }
    context.setPolygonMode(CullFace::FrontBack, PolygonMode::Fill);
}

void RendererHelper::render(Context& context, const Assets& assets, const Camera& camera, const Lighting& lighting, Instances& instances) {
    if (settings.bounding_box) {
        renderBoundingBoxes(context, assets, instances);
    }

    if (settings.coordinate_system_axes) {
        renderCoordinateSystemAxes(context, assets);
    }

    if (settings.light_radius) {
        renderLightsVolume(context, lighting, assets, camera);
    }
}
