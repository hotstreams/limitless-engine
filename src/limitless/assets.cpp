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
#include <iostream>

using namespace Limitless;

Assets::Assets(const fs::path& _base_dir) noexcept
	: base_dir {_base_dir}
	, shader_dir {_base_dir / "../shaders_ref"} {
}

Assets::Assets(fs::path _base_dir, fs::path _shader_dir) noexcept
    : base_dir {std::move(_base_dir)}
    , shader_dir {std::move(_shader_dir)} {
}

void Assets::load([[maybe_unused]] Context& context) {
    // builds default materials for every model type
    ms::Material::builder()
        .name("default")
        .shading(ms::Shading::Unlit)
        .color({0.7f, 0.0f, 0.7f, 1.0f})
        .models({InstanceType::Model, InstanceType::Skeletal, InstanceType::Effect, InstanceType::Instanced })
        .two_sided(true)
        .build(*this);

    ms::Material::builder()
            .name("red")
            .color({1.0f, 0.0f, 0.0f, 1.0f})
            .models({InstanceType::Model, InstanceType::Skeletal, InstanceType::Effect, InstanceType::Instanced })
            .two_sided(true)
            .build(*this);

    ms::Material::builder()
            .name("blue")
            .color({0.0f, 0.0f, 1.0f, 1.0f})
            .models({InstanceType::Model, InstanceType::Skeletal, InstanceType::Effect, InstanceType::Instanced })
            .two_sided(true)
            .build(*this);

    ms::Material::builder()
            .name("green")
            .color({0.0f, 1.0f, 0.0f, 1.0f})
            .models({InstanceType::Model, InstanceType::Skeletal, InstanceType::Effect, InstanceType::Instanced })
            .two_sided(true)
            .build(*this);

    // used in render as point light model
    models.add("sphere", std::make_shared<Sphere>(glm::uvec2{32}));
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

void Assets::compileAssets(Context& ctx, const RenderSettings& settings) {
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

void Assets::recompileAssets(Context& ctx, const RenderSettings& settings) {
    shaders.clear();
    compileAssets(ctx, settings);
}

void Assets::compileMaterial(Context& ctx, const RenderSettings& settings, const std::shared_ptr<ms::Material>& material) {
    ms::MaterialCompiler compiler {ctx, *this, settings};

    for (const auto& model_shader_type : material->getModelShaders()) {
        // effect shaders compiled separately
        if (model_shader_type == InstanceType::Effect) {
            continue;
        }

        for (const auto& pass_shader : getRequiredPassShaders(settings)) {
            if (!shaders.reserveIfNotContains(pass_shader, model_shader_type, material->getShaderIndex())) {
                compiler.compile(*material, pass_shader, model_shader_type);
            }
        }
    }
}

ShaderTypes Assets::getRequiredPassShaders(const RenderSettings& settings) {
    ShaderTypes pass_shaders;

    if (settings.pipeline == RenderPipeline::Forward) {
        pass_shaders.emplace(ShaderType::Forward);
    }

    if (settings.pipeline == RenderPipeline::Deferred) {
        pass_shaders.emplace(ShaderType::Depth);
        pass_shaders.emplace(ShaderType::GBuffer);
        //for transparent pass?
        pass_shaders.emplace(ShaderType::Forward);
    }

    if (settings.cascade_shadow_maps) {
        pass_shaders.emplace(ShaderType::DirectionalShadow);
    }

    pass_shaders.emplace(ShaderType::ColorPicker);

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

    if (!shaders.contains(ShaderType::Skybox, InstanceType::Model, skybox->getMaterial().getShaderIndex())) {
        compiler.compile(skybox->getMaterial(), ShaderType::Skybox, InstanceType::Model);
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

void Assets::recompileMaterial(Context& ctx, const RenderSettings& settings, const std::shared_ptr<ms::Material>& material) {
    ms::MaterialCompiler compiler {ctx, *this, settings};

    for (const auto& model_shader_type : material->getModelShaders()) {
        // effect shaders compiled separately
        if (model_shader_type == InstanceType::Effect) {
            continue;
        }

        for (const auto& pass_shader : getRequiredPassShaders(settings)) {
            shaders.remove(pass_shader, model_shader_type, material->getShaderIndex());
            compiler.compile(*material, pass_shader, model_shader_type);
        }
    }
}
