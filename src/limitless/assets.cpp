#include <limitless/assets.hpp>

#include <limitless/ms/material_builder.hpp>
#include <limitless/ms/material_compiler.hpp>
#include <limitless/fx/effect_compiler.hpp>
#include <limitless/skybox/skybox.hpp>

#include <limitless/models/sphere.hpp>
#include <limitless/models/quad.hpp>
#include <limitless/models/cube.hpp>
#include <limitless/models/plane.hpp>
#include <limitless/models/line.hpp>
#include <limitless/models/cylinder.hpp>

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

void Assets::load([[maybe_unused]] Context& context) {
    // builds default materials for every model type
    ms::MaterialBuilder builder {*this};
    ModelShaders model_types = { ModelShader::Model, ModelShader::Skeletal, ModelShader::Effect, ModelShader::Instanced };
    builder.setName("default").setShading(ms::Shading::Unlit).add(ms::Property::Color, {0.7f, 0.0f, 0.7f, 1.0f}).setModelShaders(model_types).setTwoSided(true).build();
    builder.setName("red").add(ms::Property::Color, {1.0f, 0.0f, 0.0f, 1.0f}).setModelShaders(model_types).setTwoSided(true).build();
    builder.setName("blue").add(ms::Property::Color, {0.0f, 0.0f, 1.0f, 1.0f}).setModelShaders(model_types).setTwoSided(true).build();
    builder.setName("green").add(ms::Property::Color, {0.0f, 1.0f, 0.0f, 1.0f}).setModelShaders(model_types).setTwoSided(true).build();

    builder.setName("outline")
    .add(ms::Property::Color, {100.0f, 0.0f, 0.0f, 1.0f})
    .setModelShaders(model_types)
    .setShading(ms::Shading::Unlit)
    .setTwoSided(true)
    .build();

    // used in render as point light model
    models.add("sphere", std::make_shared<Sphere>(glm::uvec2{50}));
    meshes.add("sphere", models.at("sphere")->getMeshes().at(0));

    // used in postprocessing
    models.add("quad", std::make_shared<Quad>());
    meshes.add("quad", models.at("quad")->getMeshes().at(0));

    // used in skybox render
    models.add("cube", std::make_shared<Cube>());
    meshes.add("cube", models.at("cube")->getMeshes().at(0));

    models.add("plane", std::make_shared<Plane>());
    meshes.add("plane", models.at("plane")->getMeshes().at(0));

    models.add("planequad", std::make_shared<PlaneQuad>());
    meshes.add("planequad", models.at("planequad")->getMeshes().at(0));

    models.add("line", std::make_shared<Line>(glm::vec3{0.0f}, glm::vec3{1.0f}));
    models.add("cylinder", std::make_shared<Cylinder>());
}

void Assets::initialize(Context& ctx, const RenderSettings& settings) {
	shaders.initialize(ctx, settings, shader_dir);
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

void Assets::compileShaders(Context& ctx, const RenderSettings& settings) {
	initialize(ctx, settings);

    for (const auto& [_, material] : materials) {
        compileMaterial(ctx, settings, material);
    }

    for (const auto& [_, effect] : effects) {
        compileEffect(ctx, settings, effect);
    }

    for (const auto& [_, skybox] : skyboxes) {
        compileSkybox(ctx, settings, skybox);
    }
}

void Assets::recompileShaders(Context& ctx, const RenderSettings& settings) {
    shaders.clear();
    compileShaders(ctx, settings);
}

void Assets::compileMaterial(Context& ctx, const RenderSettings& settings, const std::shared_ptr<ms::Material>& material) {
    ms::MaterialCompiler compiler {ctx, *this, settings};

    for (const auto& model_shader_type : material->getModelShaders()) {
        // effect shaders compiled separately
        if (model_shader_type == ModelShader::Effect) {
            continue;
        }

        for (const auto& pass_shader : getRequiredPassShaders(settings)) {
            if (!shaders.reserveIfNotContains(pass_shader, model_shader_type, material->getShaderIndex())) {
                compiler.compile(*material, pass_shader, model_shader_type);
            }
        }
    }
}

PassShaders Assets::getRequiredPassShaders(const RenderSettings& settings) {
    PassShaders pass_shaders;

//    if (settings.pipeline == RenderPipeline::Forward) {
        pass_shaders.emplace(ShaderPass::Forward);
//    }

    if (settings.pipeline == RenderPipeline::Deferred) {
        pass_shaders.emplace(ShaderPass::Depth);
        pass_shaders.emplace(ShaderPass::GBuffer);
        pass_shaders.emplace(ShaderPass::Transparent);
    }

    if (settings.directional_cascade_shadow_mapping) {
        pass_shaders.emplace(ShaderPass::DirectionalShadow);
    }

    pass_shaders.emplace(ShaderPass::ColorPicker);

//    #ifdef LIMITLESS_DEBUG
//        pass_shaders.emplace(ShaderPass::Forward);
//    #endif

    return pass_shaders;
}

void Assets::compileEffect(Context& ctx, const RenderSettings& settings, const std::shared_ptr<EffectInstance>& effect) {
    fx::EffectCompiler compiler {ctx, *this, settings};

    for (const auto& pass_shader : getRequiredPassShaders(settings)) {
        compiler.compile(*effect, pass_shader);
    }
}

void Assets::compileSkybox(Context& ctx, const RenderSettings& settings, const std::shared_ptr<Skybox>& skybox) {
    ms::MaterialCompiler compiler {ctx, *this, settings};

    if (!shaders.contains(ShaderPass::Skybox, ModelShader::Model, skybox->getMaterial().getShaderIndex())) {
        compiler.compile(skybox->getMaterial(), ShaderPass::Skybox, ModelShader::Model);
    }
}

void Assets::reloadTextures(const TextureLoaderFlags& settings) {
	for (auto it = textures.begin(); it != textures.end();) {
		const auto [name, texture] = *it;
		const auto path = texture->getPath().value();

		it = textures.remove(it);

		TextureLoader::load(*this, path, settings);
	}
}
