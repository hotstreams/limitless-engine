#include <limitless/assets.hpp>

#include <limitless/ms/material_builder.hpp>
#include <limitless/ms/material_compiler.hpp>
#include <limitless/fx/effect_compiler.hpp>
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

void Assets::load(Context& context) {
    // engine-required assets
    shaders.initialize(context, getShaderDir());

    // builds default materials for every model type
    ms::MaterialBuilder builder {context, *this};
    ModelShaders model_types = { ModelShader::Model, ModelShader::Skeletal, ModelShader::Effect, ModelShader::Instanced };
    builder.setName("default").add(ms::Property::Color, {0.0f, 0.0f, 0.0f, 1.0f}).setModelShaders(model_types).build();
    builder.setName("red").add(ms::Property::Color, {1.0f, 0.0f, 0.0f, 1.0f}).setModelShaders(model_types).build();
    builder.setName("blue").add(ms::Property::Color, {0.0f, 0.0f, 1.0f, 1.0f}).setModelShaders(model_types).build();
    builder.setName("green").add(ms::Property::Color, {0.0f, 1.0f, 0.0f, 1.0f}).setModelShaders(model_types).build();

    // used in render as point light model
    models.add("sphere", std::make_shared<Sphere>(100, 100));
    meshes.add("sphere", models.at("sphere")->getMeshes().at(0));

    // used in postprocessing
    models.add("quad", std::make_shared<Quad>());
    meshes.add("quad", models.at("quad")->getMeshes().at(0));

    // used in skybox render
    models.add("cube", std::make_shared<Cube>());
    meshes.add("cube", models.at("cube")->getMeshes().at(0));

    models.add("plane", std::make_shared<Plane>());
    meshes.add("plane", models.at("plane")->getMeshes().at(0));
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
    // constructing pass shaders based on settings
    PassShaders pass_shaders;
    {
        // adds current renderer
        pass_shaders.emplace(settings.renderer);

        // adds shadow pass
        if (settings.directional_csm) {
            pass_shaders.emplace(ShaderPass::DirectionalShadow);
        }
    }

    // compiles shaders for materials
    {
        ms::MaterialCompiler compiler {ctx, *this, settings};

        // iterating over every material
        for (const auto& [name, material] : materials) {
            // iterating over required model types
            for (const auto& model_shader_type : material->getModelShaders()) {
                // effect shaders compiled separately
                if (model_shader_type == ModelShader::Effect) {
                    continue;
                }

                // iterating over required render pass
                for (const auto& pass_shader : pass_shaders) {
                    if (!shaders.contains(pass_shader, model_shader_type, material->getShaderIndex())) {
                        compiler.compile(*material, pass_shader, model_shader_type);
                    }
                }
            }
        }
    }

    // compiles effect shaders
    {
        fx::EffectCompiler compiler {ctx, *this, settings};

        // iterating over every effect
        for (const auto& [name, effect] : effects) {
            // iterating over required render pass
            for (const auto& pass_shader : pass_shaders) {
                compiler.compile(*effect, pass_shader);
            }
        }
    }

    // compiles skybox shaders
    {
        ms::MaterialCompiler compiler {ctx, *this, settings};

        for (const auto& [name, skybox] : skyboxes) {
            if (!shaders.contains(ShaderPass::Skybox, ModelShader::Model, skybox->getMaterial().getShaderIndex())) {
                compiler.compile(skybox->getMaterial(), ShaderPass::Skybox, ModelShader::Model);
            }
        }
    }
}

void Assets::recompileShaders(Context& ctx, const RenderSettings& settings) {
    shaders.clearEffectShaders();
    shaders.clearMaterialShaders();

    compileShaders(ctx, settings);
}
