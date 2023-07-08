#include <limitless/renderer/renderer.hpp>

#include <limitless/pipeline/pipeline.hpp>
#include <limitless/assets.hpp>

#include <limitless/ms/material_compiler.hpp>
#include <limitless/core/context_observer.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/instances/effect_instance.hpp>
#include <limitless/pipeline/forward/forward.hpp>
#include <limitless/pipeline/deferred/deferred.hpp>

#include <limitless/core/profiler.hpp>

using namespace Limitless;

Renderer::Renderer(std::unique_ptr<Pipeline> _pipeline, const RenderSettings& _settings)
    : settings {_settings}
    , pipeline {std::move(_pipeline)} {
}

Renderer::Renderer(ContextEventObserver& ctx, const RenderSettings& _settings)
	: settings {_settings}
	, pipeline {std::make_unique<Deferred>(ctx, ctx.getSize(), settings)} {
}

Renderer::Renderer(ContextEventObserver& ctx)
    : settings {}
    , pipeline {std::make_unique<Deferred>(ctx, ctx.getSize(), settings)} {
}

void Renderer::draw(Context& context, const Assets& assets, Scene& scene, Camera& camera) {
    pipeline->draw(context, assets, scene, camera);

    //profiler.draw(context, assets);
}

void Renderer::updatePipeline(ContextEventObserver& ctx) {
    pipeline->update(ctx, settings);
}

void Renderer::update(ContextEventObserver& ctx, Assets& assets) {
    assets.recompileAssets(ctx, settings);
	updatePipeline(ctx);
}
