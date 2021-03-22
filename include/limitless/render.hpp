#pragma once

#include <limitless/particle_system/effect_renderer.hpp>
#include <limitless/postprocessing.hpp>
#include <limitless/scene_data.hpp>

namespace LimitlessEngine {
    enum class MaterialShader;
    class AbstractInstance;
    enum class Blending;

    class Renderer {
    public:
        PostProcessing postprocess;
    private:
        EffectRenderer effect_renderer;
        SceneDataStorage scene_data;
        Framebuffer offscreen;

        void dispatchInstances(const std::vector<AbstractInstance*>& instances, Context& context, const Assets& assets, MaterialShader shader_type, Blending blending) const;
        void renderLightsVolume(Context& context, Scene& scene) const;
        void initializeOffscreenBuffer(ContextEventObserver& ctx);

        std::vector<AbstractInstance*> performFrustumCulling(Scene& scene, Camera& camera) const noexcept;
    public:
        explicit Renderer(ContextEventObserver& context);

        void draw(Context& context, const Assets& assets, Scene& scene, Camera& camera);
    };
}
