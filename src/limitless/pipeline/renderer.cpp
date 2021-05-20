#include <limitless/pipeline/renderer.hpp>

#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/render_pass.hpp>
#include <limitless/assets.hpp>

#include <limitless/ms/material_compiler.hpp>
#include <limitless/core/context_observer.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/instances/effect_instance.hpp>
#include <limitless/fx/effect_compiler.hpp>

using namespace Limitless;

Renderer::Renderer(std::unique_ptr<Pipeline> _pipeline, const RenderSettings& _settings)
    : pipeline {std::move(_pipeline)}
    , settings {_settings} {
}

void Renderer::draw(Context& context, const Assets& assets, Scene& scene, Camera& camera) {
    pipeline->draw(context, assets, scene, camera);
}

void Renderer::update(ContextEventObserver& ctx, Assets& assets, Scene& scene) {
//    assets.shaders.clearMaterialShaders();
//    ms::MaterialCompiler compiler {ctx, assets, settings};
//    for (const auto& [name, material] : assets.materials) {
//        for (const auto& model : material->getModelShaders()) {
//            for (const auto& pass : material->getPassShaders()) {
//                if (!assets.shaders.contains(pass, model, material->getShaderIndex())) {
//                    compiler.compile(*material, pass, model);
//                }
//            }
//        }
//    }
//
//    assets.shaders.clearEffectShaders();
//    fx::EffectCompiler fx_compiler {ctx, assets, settings};
//    for (const auto& [name, effect] : assets.effects) {
//        fx_compiler.compile(*effect, settings.renderer);
//        if (settings.directional_csm) {
//            fx_compiler.compile(*effect, ShaderPass::DirectionalShadow);
//        }
//    }
//
//    pipeline->update(ctx, scene, settings);
}
