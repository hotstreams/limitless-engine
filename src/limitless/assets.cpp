#include <limitless/assets.hpp>

#include <limitless/ms/material_builder.hpp>
#include <limitless/skybox/skybox.hpp>

#include <limitless/models/sphere.hpp>
#include <limitless/models/quad.hpp>
#include <limitless/models/cube.hpp>
#include <limitless/models/plane.hpp>

using namespace Limitless;

Assets::Assets(std::filesystem::path _base_dir)
	: base_dir {std::move(_base_dir)}
{
}

void Assets::load(Context& context, const RenderSettings& settings) {
    // engine-required assets
    shaders.initialize(context, getShaderDir());

    // used in render as light radius material
    ms::MaterialBuilder builder {context, *this, settings};
    builder.setName("default").add(ms::Property::Color, {0.0f, 0.0f, 0.0f, 1.0f}).build();
    builder.setName("red").add(ms::Property::Color, {1.0f, 0.0f, 0.0f, 1.0f}).build();
    builder.setName("blue").add(ms::Property::Color, {0.0f, 0.0f, 1.0f, 1.0f}).build();
    builder.setName("green").add(ms::Property::Color, {0.0f, 1.0f, 0.0f, 1.0f}).build();

    // used in render as point light model
    models.add("sphere", std::make_shared<Sphere>(100, 100));
    meshes.add("sphere_mesh", models.at("sphere")->getMeshes().at(0));

    // used in postprocessing
    models.add("quad", std::make_shared<Quad>());
    meshes.add("quad_mesh", models.at("quad")->getMeshes().at(0));

    // used in skybox render
    models.add("cube", std::make_shared<Cube>());
    meshes.add("cube_mesh", models.at("cube")->getMeshes().at(0));

    models.add("plane", std::make_shared<Plane>());
    meshes.add("plane_mesh", models.at("plane")->getMeshes().at(0));
}

std::filesystem::path Assets::getShaderDir() const noexcept {
	return base_dir/"../shaders";
}
