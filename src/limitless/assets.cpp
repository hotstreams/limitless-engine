#include <limitless/assets.hpp>

#include <limitless/ms/material_builder.hpp>
#include <limitless/skybox/skybox.hpp>

#include <limitless/models/sphere.hpp>
#include <limitless/models/quad.hpp>
#include <limitless/models/cube.hpp>
#include <limitless/models/plane.hpp>
#include <utility>

using namespace Limitless;

Assets::Assets(const fs::path& _base_dir) noexcept
	: base_dir {_base_dir}
	, shader_dir {_base_dir / "../shaders"} {
}

Assets::Assets(fs::path _base_dir, fs::path _shader_dir) noexcept
    : base_dir {std::move(_base_dir)}
    , shader_dir {std::move(_shader_dir)} {
}

void Assets::load(Context& context, const RenderSettings& settings) {
    // engine-required assets
    shaders.initialize(context, getShaderDir());

    // used in render as light radius material
    ms::MaterialBuilder builder {context, *this, settings};
    builder.setName("default").add(ms::Property::Color, {0.0f, 0.0f, 0.0f, 1.0f}).build();
    builder.setName("red").add(ms::Property::Color, {1.0f, 0.0f, 0.0f, 1.0f}).build();
    builder.setName("blue").add(ms::Property::Color, {0.0f, 0.0f, 1.0f, 1.0f}).build();
    builder.setName("green").add(ms::Property::Color, {0.0f, 1.0f, 0.0f, 1.0f}).setModelShaders({ModelShader::Model, ModelShader::Skeletal}).build();

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

void Assets::add(const Assets& other) {
    shaders.add(other.shaders);
    models.add(other.models);
    meshes.add(other.meshes);
    textures.add(other.textures);
    materials.add(other.materials);
    skyboxes.add(other.skyboxes);
    effects.add(other.effects);
    fonts.add(other.fonts);
}
