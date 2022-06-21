#include "sponza_demoscene.hpp"

#include <limitless/instances/model_instance.hpp>
#include <limitless/ms/material.hpp>

using namespace LimitlessDemo;
using namespace Limitless;

SponzaScene::SponzaScene(Limitless::Context& ctx, Limitless::Assets& assets)
    : Limitless::Scene(ctx) {

    setSkybox(assets.skyboxes.at("skybox"));

    lighting.directional_light = {glm::vec4(0.0, -1.0, 0.25, 1.0f), glm::vec4{0.5f, 1.1f, 1.0f, 1.0f}};

    auto& sponza = dynamic_cast<ModelInstance&>(add<ModelInstance>(assets.models.at("sponza"), glm::vec3(17.0f, 8.0f, 15.0f))
            .setScale(glm::vec3(0.005f)));

    for (auto& [name, mesh] : sponza.getMeshes()) {
        mesh.getMaterial()[0].getTwoSided() = true;
        mesh.getMaterial()[0].getBlending() = Limitless::ms::Blending::Opaque;
    }
}

void SponzaScene::update(Limitless::Context& context, const Limitless::Camera& camera) {
    Limitless::Scene::update(context, camera);
}