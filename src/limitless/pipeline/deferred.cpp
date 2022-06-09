#include <limitless/pipeline/deferred.hpp>

#include <limitless/core/context_observer.hpp>
#include <limitless/ms/blending.hpp>

#include <limitless/pipeline/sceneupdate_pass.hpp>
#include <limitless/pipeline/effectupdate_pass.hpp>
#include <limitless/pipeline/shadow_pass.hpp>
#include <limitless/pipeline/skybox_pass.hpp>
#include <limitless/pipeline/postprocessing_pass.hpp>
#include <limitless/pipeline/deferred_framebuffer_pass.hpp>
#include <limitless/pipeline/deferred_lighting_pass.hpp>
#include <limitless/pipeline/translucent_pass.hpp>
#include <limitless/pipeline/gbuffer_pass.hpp>
#include <limitless/pipeline/depth_pass.hpp>
#include <limitless/scene.hpp>
#include <limitless/pipeline/blur_pass.hpp>
#include <limitless/pipeline/composite_pass.hpp>
#include <limitless/pipeline/ssao_pass.hpp>
#include <limitless/pipeline/fxaa_pass.hpp>
#include <limitless/pipeline/quad_pass.hpp>
#include <limitless/pipeline/render_debug_pass.hpp>
#include <limitless/core/texture_builder.hpp>
#include <limitless/pipeline/dof_pass.hpp>
#include <limitless/pipeline/outline_pass.hpp>

using namespace Limitless;

Deferred::Deferred(ContextEventObserver& ctx, const RenderSettings& settings)
	: target {default_framebuffer} {
    create(ctx, settings);
}

Deferred::Deferred(ContextEventObserver& ctx, const RenderSettings& settings, RenderTarget& _target)
    : target {_target} {
    create(ctx, settings);
}

void Deferred::update(ContextEventObserver& ctx, const RenderSettings& settings) {
    clear();
    create(ctx, settings);
}

void Deferred::create(ContextEventObserver& ctx, const RenderSettings& settings) {
    add<SceneUpdatePass>(ctx);
    auto& fx = add<EffectUpdatePass>(ctx);

    if (settings.directional_cascade_shadow_mapping) {
        add<DirectionalShadowPass>(ctx, settings, fx.getRenderer());
    }

    add<DeferredFramebufferPass>(ctx);
    add<DepthPass>(fx.getRenderer());
    add<GBufferPass>(fx.getRenderer());

    add<SkyboxPass>();

    if (settings.screen_space_ambient_occlusion) {
        add<SSAOPass>(ctx);
    }

    add<DeferredLightingPass>();

    add<TranslucentPass>(fx.getRenderer());

    add<BlurPass>(ctx);

    add<CompositePass>(ctx);

//    add<OutlinePass>();

    if (settings.fast_approximate_antialiasing) {
        add<FXAAPass>(ctx);
    }

    if (settings.depth_of_field) {
    	add<DoFPass>(ctx);
    }

	add<QuadPass>(target);

//    #ifdef LIMITLESS_DEBUG
//        add<RenderDebugPass>(settings);
//    #endif
}
