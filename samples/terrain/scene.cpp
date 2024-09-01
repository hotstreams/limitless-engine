#include "scene.hpp"

#include <limitless/scene.hpp>
#include <limitless/instances/instanced_instance.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/lighting/light.hpp>
#include <random>
#include <limitless/core/vertex.hpp>
#include <limitless/instances/terrain_instance.hpp>

using namespace LimitlessMaterials;
using namespace Limitless;

LimitlessMaterials::Scene::Scene(Limitless::Context& ctx, Limitless::Assets& assets)
    : scene(ctx)
    , assets {assets} {

    scene.add(Instance::builder()
        .model(assets.models.at("cube"))
        .position({0.0f, 0.0f, 0.0f})
        .material(assets.materials.at("red"))
        .build()
    );

    scene.add(
        TerrainInstance::create(assets)
    );
}

void LimitlessMaterials::Scene::update(Limitless::Context& context, const Limitless::Camera& camera) {

}