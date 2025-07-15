#include <limitless/renderer/renderer.hpp>

#include <limitless/assets.hpp>

#include <limitless/ms/material_compiler.hpp>
#include <limitless/core/context.hpp>
#include <limitless/instances/effect_instance.hpp>

#include <limitless/core/profiler.hpp>
#include <limitless/renderer/sceneupdate_pass.hpp>
#include <limitless/renderer/shadow_pass.hpp>
#include <limitless/renderer/depth_pass.hpp>
#include <limitless/renderer/gbuffer_pass.hpp>
#include <limitless/renderer/decal_pass.hpp>
#include <limitless/renderer/skybox_pass.hpp>
#include <limitless/renderer/deferred_lighting_pass.hpp>
#include <limitless/renderer/translucent_pass.hpp>
#include <limitless/renderer/bloom_pass.hpp>
#include <limitless/renderer/composite_pass.hpp>
#include <limitless/renderer/screen_pass.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>
#include <limitless/renderer/outline_pass.hpp>
#include <limitless/renderer/render_debug_pass.hpp>
#include <limitless/renderer/color_picker.hpp>
#include <limitless/renderer/ssao_pass.hpp>
#include <limitless/renderer/ssr_pass.hpp>
#include <limitless/renderer/fxaa_pass.hpp>
#include <limitless/core/cpu_profiler.hpp>

using namespace Limitless;

void Renderer::render(Context& context, const Assets& assets, Scene& scene, Camera& camera) {
    CpuProfileScope scope(global_profiler, "Renderer::render");

    instance_renderer.update(scene, camera);

    {
        CpuProfileScope scope(global_profiler, "Renderer::render_update_passes");

        for (const auto& pass: passes) {
            pass->update(scene, camera);
        }
    }

    {
        CpuProfileScope scope(global_profiler, "Renderer::render_render_passes");

        UniformSetter setter;
        for (const auto& pass: passes) {
            pass->render(instance_renderer, scene, context, assets, camera, setter);
            pass->addUniformSetter(setter);
        }
    }
}

void Renderer::onFramebufferChange(glm::uvec2 size) {
    resolution = size;

    for (const auto& pass: passes) {
        pass->onFramebufferChange(resolution);
    }
}

void Renderer::update(const RendererSettings& rsettings) {
    settings = rsettings;

    Builder {*this}
        .update()
        .build(*this);

    for (const auto& pass: passes) {
        pass->update(settings);
    }
}

Renderer::Builder& Renderer::Builder::addSceneUpdatePass() {
    renderer->passes.emplace_back(std::make_unique<SceneUpdatePass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addDirectionalShadowPass() {
    renderer->passes.emplace_back(std::make_unique<DirectionalShadowPass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addDeferredFramebufferPass() {
    renderer->passes.emplace_back(std::make_unique<DeferredFramebufferPass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addDepthPass() {
    renderer->passes.emplace_back(std::make_unique<DepthPass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addColorPicker() {
    renderer->passes.emplace_back(std::make_unique<ColorPicker>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addGBufferPass() {
    renderer->passes.emplace_back(std::make_unique<GBufferPass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addDecalPass() {
    renderer->passes.emplace_back(std::make_unique<DecalPass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addSkyboxPass() {
    renderer->passes.emplace_back(std::make_unique<SkyboxPass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addSSAOPass() {
    renderer->passes.emplace_back(std::make_unique<SSAOPass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addSSRPass() {
    renderer->passes.emplace_back(std::make_unique<SSRPass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addDeferredLightingPass() {
    renderer->passes.emplace_back(std::make_unique<DeferredLightingPass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addTranslucentPass() {
    renderer->passes.emplace_back(std::make_unique<TranslucentPass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addBloomPass() {
    renderer->passes.emplace_back(std::make_unique<BloomPass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addOutlinePass() {
    renderer->passes.emplace_back(std::make_unique<OutlinePass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addCompositeWithBloomPass() {
    renderer->passes.emplace_back(std::make_unique<CompositeWithBloomPass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addCompositePass() {
    renderer->passes.emplace_back(std::make_unique<CompositePass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addFXAAPass() {
    renderer->passes.emplace_back(std::make_unique<FXAAPass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addScreenPass() {
    renderer->passes.emplace_back(std::make_unique<ScreenPass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::addRenderDebugPass() {
    renderer->passes.emplace_back(std::make_unique<RenderDebugPass>(*renderer));
    return *this;
}

Renderer::Builder &Renderer::Builder::deferred() {
    addSceneUpdatePass();
    if (renderer->settings.cascade_shadow_maps) {
        addDirectionalShadowPass();
    }
    addDeferredFramebufferPass();
    addDepthPass();
    addColorPicker();
    addGBufferPass();
    addDecalPass();
    addSkyboxPass();
    if (renderer->settings.screen_space_ambient_occlusion) {
        addSSAOPass();
    }
    if (renderer->settings.screen_space_reflections) {
        addSSRPass();
    }
    addDeferredLightingPass();
    addTranslucentPass();
    if (renderer->settings.bloom) {
        addBloomPass();
    }
    addOutlinePass();
    if (renderer->settings.bloom) {
        addCompositeWithBloomPass();
    } else {
        addCompositePass();
    }
    if (renderer->settings.fast_approximate_antialiasing) {
        addFXAAPass();
    }
    addScreenPass();
    if (renderer->settings.bounding_box || renderer->settings.light_radius || renderer->settings.coordinate_system_axes) {
        addRenderDebugPass();
    }
    return *this;
}

std::unique_ptr<Renderer> Renderer::Builder::build() {
    return std::move(renderer);
}

Renderer::Builder &Renderer::Builder::resolution(glm::uvec2 resolution) {
    renderer->resolution = resolution;
    return *this;
}

Renderer::Builder &Renderer::Builder::settings(const RendererSettings &settings) {
    renderer->settings = settings;
    return *this;
}

Renderer::Builder::Builder()
    : renderer {new Renderer()} {
}

Renderer::Builder & Renderer::Builder::update() {
    auto& settings = renderer->settings;

    if (settings.cascade_shadow_maps) {
        if (!renderer->isPresent<DirectionalShadowPass>()) {
            addAfter<SceneUpdatePass, DirectionalShadowPass>();
        }
    } else {
        remove<DirectionalShadowPass>();
    }

//    if (settings.screen_space_ambient_occlusion) {
//        if (!renderer->isPresent<SSAOPass>()) {
//            addAfter<SkyboxPass>(std::make_unique<SSAOPass>(*renderer));
//        }
//    } else {
//      remove<SSAOPass>();
//    }

//    if (settings.screen_space_reflections) {
//        if (!renderer->isPresent<SSRPass>()) {
//            if (renderer->isPresent<SSAOPass>()) {
//                addAfter<SSAOPass>(std::make_unique<SSRPass>(*renderer));
//            } else {
//                addAfter<addSkyboxPass>(std::make_unique<SSRPass>(*renderer));
//            }
//        }
//    } else {
//        remove<SSRPass>();
//    }

    if (settings.bloom) {
        if (!renderer->isPresent<BloomPass>()) {
            addAfter<TranslucentPass, BloomPass>();
        }
    } else {
        remove<BloomPass>();
    }

//    if (settings.bloom) {
//        if (!renderer->isPresent<FXAAPass>()) {
//            addAfter<CompositePass>(std::make_unique<FXAAPass>(*renderer));
//        }
//    } else {
//        remove<FXAAPass>();
//    }

    if (settings.bounding_box || settings.light_radius || settings.coordinate_system_axes) {
        if (!renderer->isPresent<RenderDebugPass>()) {
            addRenderDebugPass();
        }
    }
    if (!settings.bounding_box && !settings.light_radius && !settings.coordinate_system_axes) {
        if (renderer->isPresent<RenderDebugPass>()) {
            remove<RenderDebugPass>();
        }
    }

    return *this;
}

Renderer::Builder::Builder(Renderer& from)
    : renderer {new Renderer()} {
    renderer->passes = std::move(from.passes);
    renderer->settings = from.settings;
    renderer->resolution = from.resolution;
}

void Renderer::Builder::build(Renderer& to) {
    to.passes = std::move(renderer->passes);
    to.settings = renderer->settings;
    to.resolution = renderer->resolution;
}
