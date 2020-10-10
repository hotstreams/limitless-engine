#pragma once

#include <postprocessing.hpp>
#include <effect_renderer.hpp>
#include <scene_data.hpp>

namespace GraphicsEngine {
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

        void dispatchInstances(const std::vector<AbstractInstance*>& instances, Context& context, MaterialShader shader_type, Blending blending) const;
        void renderLightsVolume(Context& context, Scene& scene) const;
        void initializeOffscreenBuffer(ContextEventObserver& ctx);

        std::vector<AbstractInstance*> performFrustumCulling(Scene& scene, Camera& camera) const noexcept;
    public:
        explicit Renderer(ContextEventObserver& context) noexcept;

        void draw(Context& context, Scene& scene, Camera& camera);
    };
}
