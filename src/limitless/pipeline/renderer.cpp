#include <limitless/pipeline/renderer.hpp>

#include <limitless/pipeline/pipeline.hpp>
#include <limitless/assets.hpp>

#include <limitless/ms/material_compiler.hpp>
#include <limitless/core/context_observer.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/instances/effect_instance.hpp>
#include <limitless/pipeline/forward.hpp>

using namespace Limitless;

Renderer::Renderer(std::unique_ptr<Pipeline> _pipeline, const RenderSettings& _settings)
    : settings {_settings}
    , pipeline {std::move(_pipeline)} {
}

Renderer::Renderer(ContextEventObserver& ctx)
    : settings {}
    , pipeline {std::make_unique<Forward>(ctx, settings)} {
}

void Renderer::draw(Context& context, const Assets& assets, Scene& scene, Camera& camera) {
    pipeline->draw(context, assets, scene, camera);
}

void Renderer::update(ContextEventObserver& ctx, Assets& assets, Scene& scene) {
    assets.recompileShaders(ctx, settings);

    pipeline->update(ctx, scene, settings);
}
