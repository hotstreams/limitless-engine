#include <limitless/pipeline/deferred/deferred.hpp>

#include <limitless/core/context.hpp>
#include <limitless/ms/blending.hpp>

#include <limitless/renderer/sceneupdate_pass.hpp>
#include <limitless/renderer/shadow_pass.hpp>
#include <limitless/renderer/skybox_pass.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>
#include <limitless/renderer/deferred_lighting_pass.hpp>
#include <limitless/renderer/translucent_pass.hpp>
#include <limitless/renderer/gbuffer_pass.hpp>
#include <limitless/renderer/depth_pass.hpp>
#include <limitless/scene.hpp>
#include <limitless/renderer/bloom_pass.hpp>
#include <limitless/renderer/composite_pass.hpp>
#include <limitless/renderer/ssao_pass.hpp>
#include <limitless/renderer/fxaa_pass.hpp>
#include <limitless/renderer/screen_pass.hpp>
#include <limitless/renderer/render_debug_pass.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/pipeline/common/dof_pass.hpp>
#include <limitless/renderer/ssr_pass.hpp>
#include <limitless/renderer/outline_pass.hpp>
#include <iostream>
#include <limitless/renderer/decal_pass.hpp>

using namespace Limitless;

Deferred::Deferred(Context& ctx, glm::uvec2 size, const RendererSettings& settings, RenderTarget& _target)
    : Pipeline {size, _target} {
    build(ctx, settings);
}

void Deferred::update(Context& ctx, const RendererSettings& settings) {
    clear();
    build(ctx, settings);
}

void Deferred::build(Context& ctx, const RendererSettings& settings) {
    /*
     * Updates scene and GPU scene buffer
     */
    add<SceneUpdatePass>(ctx);

    /*
     * Fills shadow maps
     */
    if (settings.cascade_shadow_maps) {
        add<DirectionalShadowPass>(ctx, settings);
    }

    /*
     * Prepares GBUFFER
     */
    add<DeferredFramebufferPass>(size);

    /*
     * Renders opaque objects to DeferredFramebufferPass's depth buffer
     */
    add<DepthPass>();

    /*
     * Fills GBUFFER with opaque objects
     */
    add<GBufferPass>();

    add<DecalPass>();

    /*
     * Renders skybox
     */
    add<SkyboxPass>();

    /*
     * TODO: ref
     */
    if (settings.screen_space_ambient_occlusion) {
        add<SSAOPass>(ctx, size);
    }

    if (settings.screen_space_reflections) {
        add<SSRPass>(ctx, size);
    }

    /*
     * Makes lighting calculations (with AO) from GBUFFER and renders result in another framebuffer
     */
    add<DeferredLightingPass>(size);

    /*
     * Copies lighting result and renders translucent objects on-top of it
     */
    add<TranslucentPass>(size, get<DeferredFramebufferPass>().getDepth());

    /*
     *  Applies bloom to previous result image
     */
    if (settings.bloom) {
        add<BloomPass>(size);
    }

    /*
     * Draws outline for objects
     */
    add<OutlinePass>(size);

    /*
     *  Combines shaded translucent result and bloom
     */
    if (settings.bloom) {
        add<CompositeWithBloomPass>(size);
    } else {
        add<CompositePass>(size);
    }

//    if (settings.fast_approximate_antialiasing) {
//        add<FXAAPass>(size);
//    }

    //TODO: refactor move to postprocessing
//    if (settings.depth_of_field) {
//        add<DoFPass>(ctx);
//    }

    add<ScreenPass>(*target);

    add<RenderDebugPass>(settings);
}
