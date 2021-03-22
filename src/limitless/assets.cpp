#include <limitless/assets.hpp>

#include <limitless/skybox.hpp>
#include <limitless/models/elementary_model.hpp>
#include <limitless/material_system/material_builder.hpp>

using namespace LimitlessEngine;

void Assets::load() {
    // engine-required assets
    shaders.initialize();

    // used in render as light radius material
    MaterialBuilder builder {*this};
    builder.create("default").add(PropertyType::Color, {0.0f, 0.0f, 0.0f, 1.0f}).build();

    // used in render as point light model
    models.add("sphere", std::make_shared<Sphere>(100, 100));
    meshes.add("sphere_mesh", models.at("sphere")->getMeshes().at(0));

    // used in postprocessing
    models.add("quad", std::make_shared<Quad>());
    meshes.add("quad_mesh", models.at("quad")->getMeshes().at(0));

    // used in skybox render
    models.add("cube", std::make_shared<Cube>());

    models.add("plane", std::make_shared<Plane>());
    meshes.add("plane_mesh", models.at("plane")->getMeshes().at(0));
}
