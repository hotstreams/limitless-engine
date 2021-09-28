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

using namespace Limitless;

Deferred::Deferred(ContextEventObserver& ctx, const RenderSettings& settings) {
    create(ctx, settings);
}

void Deferred::update(ContextEventObserver& ctx, [[maybe_unused]] Scene& scene, const RenderSettings& settings) {
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

    if (settings.fast_approximate_antialiasing) {
        add<FXAAPass>();
    } else {
        add<QuadPass>();
    }

    #ifdef LIMITLESS_DEBUG
        add<RenderDebugPass>(settings);
    #endif
}
