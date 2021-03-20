#include <assets.hpp>

#include <skybox.hpp>
#include <elementary_model.hpp>
#include <model_loader.hpp>
#include <material_system/material_builder.hpp>

using namespace LimitlessEngine;

void Assets::load() {
    // engine-required assets

    // used in render as light radius material
    MaterialBuilder builder;
    builder.create("default").add(PropertyType::Color, {0.0f, 0.0f, 0.0f, 1.0f}).build();

    // used in render as point light model
    models.add("sphere", std::make_shared<Sphere>(100, 100));
    // used in postprocessing
    models.add("quad", std::make_shared<Quad>());
    // used in skybox render
    models.add("cube", std::make_shared<Cube>());

    models.add("rectangle", std::make_shared<Rectangle>());
}
