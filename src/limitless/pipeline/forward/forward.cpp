#include <limitless/pipeline/forward/forward.hpp>

#include <limitless/core/context.hpp>
#include <limitless/ms/blending.hpp>

#include <limitless/pipeline/common/sceneupdate_pass.hpp>
#include <limitless/pipeline/deferred/effectupdate_pass.hpp>
#include <limitless/pipeline/common/shadow_pass.hpp>
#include <limitless/pipeline/forward/framebuffer_pass.hpp>
#include <limitless/pipeline/forward/color_pass.hpp>
#include <limitless/pipeline/forward/particle_pass.hpp>
#include <limitless/pipeline/common/skybox_pass.hpp>
#include <limitless/pipeline/forward/postprocessing_pass.hpp>
#include <limitless/scene.hpp>

using namespace Limitless;

//Forward::Forward(Context& ctx, const RenderSettings& settings) {
//    create(ctx, settings);
//}
//
//void Forward::update(Context& ctx, const RenderSettings& settings) {
//    clear();
//    create(ctx, settings);
//}
//
//void Forward::create(Context& ctx, const RenderSettings& settings) {
//    add<SceneUpdatePass>(ctx);
//    auto& fx = add<EffectUpdatePass>(ctx);
//
//    if (settings.cascade_shadow_maps) {
//        add<DirectionalShadowPass>(ctx, settings, fx.getRenderer());
//    }
//
//    add<FramebufferPass>(ctx);
//    add<ColorPass>(ms::Blending::Opaque);
//    add<ParticlePass>(fx.getRenderer(), ms::Blending::Opaque);
//    add<SkyboxPass>();
//    add<ColorPass>(ms::Blending::Additive);
//    add<ParticlePass>(fx.getRenderer(), ms::Blending::Additive);
//    add<ColorPass>(ms::Blending::Modulate);
//    add<ParticlePass>(fx.getRenderer(), ms::Blending::Modulate);
//    add<ColorPass>(ms::Blending::Translucent);
//    add<ParticlePass>(fx.getRenderer(), ms::Blending::Translucent);
//    add<PostEffectsPass>(ctx);
//}
