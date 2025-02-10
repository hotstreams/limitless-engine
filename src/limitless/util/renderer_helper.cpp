#include <cmath>
#include <limitless/util/renderer_helper.hpp>

#include <limitless/renderer/renderer_settings.hpp>
#include <limitless/scene.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/context.hpp>

#include <limitless/ms/blending.hpp>
#include <limitless/models/line.hpp>
#include <limitless/instances/model_instance.hpp>
#include <limitless/models/cylinder.hpp>
#include <limitless/renderer/instance_renderer.hpp>
#include <iostream>

using namespace Limitless;
using namespace Limitless::ms;

RendererHelper::RendererHelper(const RendererSettings& _settings)
    : settings {_settings} {
}

void RendererHelper::renderLightsVolume(Context& context, const Lighting& lighting, const Assets& assets, const Camera& camera) {
    if (lighting.getLights().empty()) {
        return;
    }

    context.enable(Capabilities::DepthTest);
    context.setDepthFunc(DepthFunc::Less);
    context.setDepthMask(DepthMask::False);
//    context.disable(Capabilities::DepthTest);

    auto sphere_instance = ModelInstance(assets.models.at("sphere"), assets.materials.at("default"), glm::vec3(0.0f));

    context.setPolygonMode(CullFace::FrontBack, PolygonMode::Line);
    for (const auto& [_, light] : lighting.getLights()) {
        if (light.isPoint()) {
            sphere_instance.setPosition(light.getPosition());
            sphere_instance.setScale(glm::vec3(light.getRadius()));
            sphere_instance.update(camera);

            InstanceRenderer::render(sphere_instance, {context, assets, ShaderType::Forward, ms::Blending::Opaque, {}});
        }
        if (light.isSpot()) {
            auto cone = std::make_shared<Cylinder>(0.0f, light.getRadius() / 1.5f * glm::sin(glm::acos(glm::radians(light.getCone().y))), light.getRadius());
            auto cone_instance = ModelInstance(cone, assets.materials.at("default"), glm::vec3(0.0f));

            cone_instance.setPosition(light.getPosition());

            auto y = glm::vec3{0.0f, 1.0f, 0.0f};
            auto a = glm::cross(y, light.getDirection());
            if (a == glm::vec3(0.0f)) {
                a = glm::vec3(1.0f, 0.0f, 0.0f);
            }
            auto angle = glm::acos(glm::dot(y, light.getDirection()));

            cone_instance.setRotation(a * angle);
            cone_instance.update(camera);

            InstanceRenderer::render(cone_instance, {context, assets, ShaderType::Forward, ms::Blending::Opaque, {}});

        }
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
    static const auto z = std::make_shared<Line>(glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 0.0f, -5.0f});

    static ModelInstance x_i {x, assets.materials.at("green"), {5.0f, 1.0f, 0.0f}};
    static ModelInstance y_i {y, assets.materials.at("blue"), {5.0f, 1.0f, 0.0f}};
    static ModelInstance z_i {z, assets.materials.at("red"), {5.0f, 1.0f, 0.0f}};

    InstanceRenderer::render(x_i, {context, assets, ShaderType::Forward, ms::Blending::Opaque, {}});
    InstanceRenderer::render(y_i, {context, assets, ShaderType::Forward, ms::Blending::Opaque, {}});
    InstanceRenderer::render(z_i, {context, assets, ShaderType::Forward, ms::Blending::Opaque, {}});
}

void RendererHelper::renderBoundingBoxes(Context& context, const Assets& assets, const Camera& camera, Scene& scene) {
    auto box = ModelInstance {assets.models.at("cube"), assets.materials.at("default"), glm::vec3{0.0f}};

    context.setLineWidth(2.5f);
    context.setPolygonMode(CullFace::FrontBack, PolygonMode::Line);
    for (const auto& instance : scene.getInstances()) {
//        auto& bounding_box = instance->getBoundingBox();
//
//        box .setPosition(bounding_box.center)
//            .setScale(bounding_box.size)
//            .update(camera);
//
//        InstanceRenderer::render(box, {context, assets, ShaderType::Forward, ms::Blending::Opaque, {}});
        if (instance->getInstanceType() == InstanceType::Terrain) {
            auto show = [&] (const auto& instance) {
                auto& bounding_box = instance->getBoundingBox();

                box .setPosition(bounding_box.center)
                        .setScale(bounding_box.size)
                        .update(camera);

                InstanceRenderer::render(box, {context, assets, ShaderType::Forward, ms::Blending::Opaque, {}});
            };

            auto& terrain = static_cast<TerrainInstance&>(*instance);


//            for (auto &item: terrain.mesh.seams) {
//                show(item);
//            }
            const auto frustum = Frustum::fromCamera(camera);
//
//            for (auto &item : terrain.mesh.trims->getInstances()[0]) {
//                if (frustum.intersects(*terrain.mesh.trims->getInstances()[0])) {
//                    show(terrain.mesh.trims->getInstances()[0]);
//                }
//            }

//            for (auto &item: terrain.mesh.fillers) {
//                show(item);
//            }

            for (auto &item: terrain.mesh.tiles->getInstances()) {
                show(item);
            }
        }
    }
    context.setPolygonMode(CullFace::FrontBack, PolygonMode::Fill);
}

void RendererHelper::render(Context& context, const Assets& assets, const Camera& camera, const Lighting& lighting, Scene& scene) {
    if (settings.bounding_box) {
        renderBoundingBoxes(context, assets, camera, scene);
    }

    if (settings.coordinate_system_axes) {
        renderCoordinateSystemAxes(context, assets);
    }

    if (settings.light_radius) {
        renderLightsVolume(context, lighting, assets, camera);
    }
}
