#pragma once

#include <limitless/particle_system/effect_renderer.hpp>
#include <limitless/lighting/shadow_mapping.hpp>
#include <limitless/postprocessing.hpp>
#include <limitless/scene_data.hpp>
#include <functional>

namespace LimitlessEngine {
    enum class MaterialShader;
    enum class Blending;
    class AbstractInstance;
    class UniformSetter;

    class Renderer {
    public:
        PostProcessing postprocess;
    private:
        EffectRenderer effect_renderer;
        SceneDataStorage scene_data;
        Framebuffer offscreen;
        ShadowMapping shadow_mapping;

        void initialize(ContextEventObserver& ctx);
        std::vector<AbstractInstance*> performFrustumCulling(Scene& scene, Camera& camera) const noexcept;

        void dispatch(Context& ctx, const Assets& assets, Scene& scene, MaterialShader shader, Blending blending, const UniformSetter& setter);
    public:
        explicit Renderer(ContextEventObserver& context);

        void draw(Context& context, const Assets& assets, Scene& scene, Camera& camera);

        void dispatch(Context& ctx, const Assets& assets, Scene& scene, MaterialShader shader, Camera& camera, const UniformSetter& setter);
    };
}
