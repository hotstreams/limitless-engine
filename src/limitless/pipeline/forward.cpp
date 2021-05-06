#include <limitless/pipeline/forward.hpp>

#include <limitless/core/context_observer.hpp>
#include <limitless/ms/blending.hpp>

#include <limitless/pipeline/sceneupdate_pass.hpp>
#include <limitless/pipeline/effectupdate_pass.hpp>
#include <limitless/pipeline/shadow_pass.hpp>
#include <limitless/pipeline/framebuffer_pass.hpp>
#include <limitless/pipeline/color_pass.hpp>
#include <limitless/pipeline/particle_pass.hpp>
#include <limitless/pipeline/skybox_pass.hpp>
#include <limitless/pipeline/postprocessing_pass.hpp>
#include <limitless/scene.hpp>
using namespace Limitless;

Forward::Forward(ContextEventObserver& ctx, Scene& scene) {
    add<SceneUpdatePass>(ctx);
    auto& fx = add<EffectUpdatePass>(ctx);
    add<DirectionalShadowPass>(ctx, scene);
    add<FramebufferPass>(ctx);
    add<ColorPass>(ms::Blending::Opaque);
    add<ParticlePass>(fx.getRenderer(), ms::Blending::Opaque);
    add<SkyboxPass>(*scene.getSkybox());
    add<ColorPass>(ms::Blending::Additive);
    add<ParticlePass>(fx.getRenderer(), ms::Blending::Additive);
    add<ColorPass>(ms::Blending::Modulate);
    add<ParticlePass>(fx.getRenderer(), ms::Blending::Modulate);
    add<ColorPass>(ms::Blending::Translucent);
    add<ParticlePass>(fx.getRenderer(), ms::Blending::Translucent);
    add<PostEffectsPass>(ctx);
}