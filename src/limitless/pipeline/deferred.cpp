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
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/pipeline/dof_pass.hpp>
#include <limitless/pipeline/outline_pass.hpp>
#include <iostream>

using namespace Limitless;

Deferred::Deferred(ContextEventObserver& ctx, glm::uvec2 size, const RenderSettings& settings, RenderTarget& _target)
    : Pipeline {size, _target} {
    build(ctx, settings);
}

void Deferred::update(ContextEventObserver& ctx, const RenderSettings& settings) {
    clear();
    build(ctx, settings);
}

void Deferred::build(ContextEventObserver& ctx, const RenderSettings& settings) {
    /*
        scene update pass ->
                                [buffer bind] scene

        shadow pass ->
                            [buffer bind] shadow

        render pass ->
                        for each instance
                            [buffer bind] skeletal

                            for each mesh
                                for each material layer
                                    set ctx state
                                    set shader program
                                    set uniform state
                                    mesh draw


     we should sort

     Renderer
        accepts scene
            |
        gets instances
            |
        for each instance
                |
            for each mesh
                    |
                for each material layer
                        |



     CommonRenderer

     BatchedRenderer

     */

    add<SceneUpdatePass>(ctx);
    auto& fx = add<EffectUpdatePass>(ctx);

    if (settings.cascade_shadow_maps) {
        add<DirectionalShadowPass>(ctx, settings, fx.getRenderer());
    }

    add<DeferredFramebufferPass>(size);
    add<DepthPass>(fx.getRenderer());
    add<GBufferPass>(fx.getRenderer());

    add<SkyboxPass>();

//    if (settings.screen_space_ambient_occlusion) {
//        add<SSAOPass>(ctx, size);
//    }

    add<DeferredLightingPass>(size);

    add<TranslucentPass>(fx.getRenderer(), size, get<DeferredFramebufferPass>().getDepth());

    add<BloomPass>(size);

    add<CompositePass>(size);

//    add<OutlinePass>();

    if (settings.fast_approximate_antialiasing) {
        add<FXAAPass>(size);
    }

//    if (settings.depth_of_field) {
//        add<DoFPass>(ctx);
//    }

    add<FinalQuadPass>(*target);
}
