#include <limitless/renderer/renderer.hpp>

#include <limitless/core/context_initializer.hpp>
#include <limitless/assets.hpp>

#include <limitless/ms/material_compiler.hpp>
#include <limitless/core/context.hpp>
#include <limitless/instances/effect_instance.hpp>
#include <limitless/core/vertex_stream/geometry_pool.hpp>

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
#include <limitless/renderer/hbao_pass.hpp>
#include <limitless/renderer/ssr_pass.hpp>
#include <limitless/renderer/fxaa_pass.hpp>

using namespace Limitless;

void Renderer::render(Context& context, const Assets& assets, Scene& scene, Camera& camera) {
    ProfilerScope profile_scope {"Renderer::render"};

    instance_renderer.update(scene, camera);

    const bool use_geometry_pool = RendererSettings::usesGeometryPool(settings);

    if (use_geometry_pool) {
        auto& pool = GeometryPool::getInstance();
        if (pool.needsUpload()) {
            pool.uploadAllToGPU();
        }
    }

#if 0 // IndirectInstanceRenderer disabled (see renderer.hpp)
    if (settings.indirect_draw) {
        const auto& visible = instance_renderer.getFrustumCulling().getVisibleInstances();
        indirect_instance_renderer.prepare(visible, camera);
        static bool debug_dumped = false;
        if (!debug_dumped) {
            indirect_instance_renderer.dumpDebugInfo();
            debug_dumped = true;
        }
    }
#endif

    {
        ProfilerScope profile_scope {"PassUpdates"};
        for (const auto& pass: passes) {
            pass->update(scene, camera);
        }
    }

    UniformSetter setter;
    {
        ProfilerScope profile_scope {"PassRenderLoop"};
        size_t pass_index = 0;
        for (const auto& pass: passes) {
            {
                pass->render(instance_renderer, scene, context, assets, camera, setter);
            }

            pass->addUniformSetter(setter);

            ++pass_index;
        }
    }

    if (use_geometry_pool && settings.persistent_buffer_mapping) {
        GeometryPool::getInstance().fenceBuffers();
    }

    global_gpu_profiler.checkPendingQueries();
}

void Renderer::onFramebufferChange(glm::uvec2 size) {
    resolution = size;

    for (const auto& pass: passes) {
        pass->onFramebufferChange(resolution);
    }
}

void Renderer::update(const RendererSettings& rsettings) {
    settings = rsettings;

    if (settings.global_model_instance_ssbo) {
        if (settings.indirect_draw || !settings.sorted_rendering || !ContextInitializer::supportsBaseInstance()
            || !ContextInitializer::supportsShaderDrawParametersGlsl()) {
            settings.global_model_instance_ssbo = false;
        }
    }
    RendererSettings::global_model_instance_ssbo_active = settings.global_model_instance_ssbo;

    RendererSettings::geometry_batching_enabled = RendererSettings::usesGeometryPool(settings);

    Builder {*this}
        .update()
        .build(*this);

    for (const auto& pass: passes) {
        pass->update(settings);
    }
}

Renderer::Builder& Renderer::Builder::addSceneUpdatePass() {
    target().passes.emplace_back(std::make_unique<SceneUpdatePass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addDirectionalShadowPass() {
    target().passes.emplace_back(std::make_unique<DirectionalShadowPass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addDeferredFramebufferPass() {
    target().passes.emplace_back(std::make_unique<DeferredFramebufferPass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addDepthPass() {
    target().passes.emplace_back(std::make_unique<DepthPass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addColorPicker() {
    target().passes.emplace_back(std::make_unique<ColorPicker>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addGBufferPass() {
    target().passes.emplace_back(std::make_unique<GBufferPass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addDecalPass() {
    target().passes.emplace_back(std::make_unique<DecalPass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addSkyboxPass() {
    target().passes.emplace_back(std::make_unique<SkyboxPass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addSSAOPass() {
    target().passes.emplace_back(std::make_unique<SSAOPass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addHBAOPass() {
    target().passes.emplace_back(std::make_unique<HBAOPass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addSSRPass() {
    target().passes.emplace_back(std::make_unique<SSRPass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addDeferredLightingPass() {
    target().passes.emplace_back(std::make_unique<DeferredLightingPass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addTranslucentPass() {
    target().passes.emplace_back(std::make_unique<TranslucentPass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addBloomPass() {
    target().passes.emplace_back(std::make_unique<BloomPass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addOutlinePass() {
    target().passes.emplace_back(std::make_unique<OutlinePass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addCompositePass() {
    target().passes.emplace_back(std::make_unique<CompositePass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addFXAAPass() {
    target().passes.emplace_back(std::make_unique<FXAAPass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addScreenPass() {
    target().passes.emplace_back(std::make_unique<ScreenPass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::addRenderDebugPass() {
    target().passes.emplace_back(std::make_unique<RenderDebugPass>(target()));
    return *this;
}

Renderer::Builder &Renderer::Builder::deferred() {
    addSceneUpdatePass();
    if (target().settings.cascade_shadow_maps) {
        addDirectionalShadowPass();
    }
    addDeferredFramebufferPass();
    addDepthPass();
    addColorPicker();
    addGBufferPass();
    addDecalPass();
    addSkyboxPass();
    if (target().settings.ambient_occlusion_mode == AmbientOcclusionMode::SAO) {
        addSSAOPass();
    } else if (target().settings.ambient_occlusion_mode == AmbientOcclusionMode::HBAO) {
        addHBAOPass();
    }
    if (target().settings.screen_space_reflections) {
        addSSRPass();
    }
    addDeferredLightingPass();
    addTranslucentPass();
    if (target().settings.bloom) {
        addBloomPass();
    }
    // addOutlinePass();
    addCompositePass();
    if (target().settings.fast_approximate_antialiasing) {
        addFXAAPass();
    }
    addScreenPass();
    if (target().settings.bounding_box || target().settings.light_radius || target().settings.coordinate_system_axes) {
        addRenderDebugPass();
    }
    return *this;
}

std::unique_ptr<Renderer> Renderer::Builder::build() {
    return std::move(owned_renderer_);
}

Renderer::Builder &Renderer::Builder::resolution(glm::uvec2 resolution) {
    target().resolution = resolution;
    return *this;
}

Renderer::Builder &Renderer::Builder::settings(const RendererSettings &settings) {
    target().settings = settings;

    RendererSettings::geometry_batching_enabled = RendererSettings::usesGeometryPool(settings);

    GeometryPool::getInstance().setUsePersistentMapping(
        RendererSettings::usesGeometryPool(settings) && settings.persistent_buffer_mapping);

#if 0
    if (settings.indirect_draw) {
        target().indirect_instance_renderer.initBuffers(
            settings.persistent_buffer_mapping,
            settings.triple_buffer_indirect);
    }
#endif

    return *this;
}

Renderer::Builder::Builder()
    : owned_renderer_(std::unique_ptr<Renderer>(new Renderer()))
    , target_(owned_renderer_.get()) {
}

Renderer::Builder & Renderer::Builder::update() {
    auto& settings = target().settings;

    if (settings.cascade_shadow_maps) {
        if (!target().isPresent<DirectionalShadowPass>()) {
            addAfter<SceneUpdatePass, DirectionalShadowPass>();
        }
    } else {
        remove<DirectionalShadowPass>();
    }

    remove<SSAOPass>();
    remove<HBAOPass>();
    if (settings.ambient_occlusion_mode == AmbientOcclusionMode::SAO) {
        addAfter<SkyboxPass, SSAOPass>();
    } else if (settings.ambient_occlusion_mode == AmbientOcclusionMode::HBAO) {
        addAfter<SkyboxPass, HBAOPass>();
    }

    if (settings.screen_space_reflections) {
        if (!target().isPresent<SSRPass>()) {
            if (target().isPresent<SSAOPass>()) {
                addAfter<SSAOPass, SSRPass>();
            } else if (target().isPresent<HBAOPass>()) {
                addAfter<HBAOPass, SSRPass>();
            } else {
                addAfter<SkyboxPass, SSRPass>();
            }
        }
    } else {
        remove<SSRPass>();
    }

    if (settings.bloom) {
        if (!target().isPresent<BloomPass>()) {
            addAfter<TranslucentPass, BloomPass>();
        }
    } else {
        remove<BloomPass>();
    }

    if (settings.fast_approximate_antialiasing) {
        if (!target().isPresent<FXAAPass>()) {
            if (target().isPresent<CompositePass>()) {
                addAfter<CompositePass, FXAAPass>();
            }
        }
    } else {
        remove<FXAAPass>();
    }

    if (settings.bounding_box || settings.light_radius || settings.coordinate_system_axes) {
        if (!target().isPresent<RenderDebugPass>()) {
            addRenderDebugPass();
        }
    }
    if (!settings.bounding_box && !settings.light_radius && !settings.coordinate_system_axes) {
        if (target().isPresent<RenderDebugPass>()) {
            remove<RenderDebugPass>();
        }
    }

    return *this;
}

Renderer::Builder::Builder(Renderer& from)
    : target_(&from) {
}

void Renderer::Builder::build(Renderer& to) {
    if (owned_renderer_) {
        to.passes = std::move(owned_renderer_->passes);
        to.settings = owned_renderer_->settings;
        to.resolution = owned_renderer_->resolution;
    }
}
