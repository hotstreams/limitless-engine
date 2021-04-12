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

    using UniformSetter = std::vector<std::function<void(ShaderProgram&)>>;

    class Renderer {
    public:
        PostProcessing postprocess;
    private:
        EffectRenderer effect_renderer;
        SceneDataStorage scene_data;
        Framebuffer offscreen;
        ShadowMapping shadow_mapping;

        void renderLightsVolume(Context& context, Scene& scene, const Assets& assets) const;
        void renderCoordinateSystemAxes(Context& context, Scene& scene, const Assets& assets) const;
        void initializeOffscreenBuffer(ContextEventObserver& ctx);

        std::vector<AbstractInstance*> performFrustumCulling(Scene& scene, Camera& camera) const noexcept;
    public:
        explicit Renderer(ContextEventObserver& context);

        void draw(Context& context, const Assets& assets, Scene& scene, Camera& camera);

        void dispatch(Scene& scene, Context& context, const Assets& assets, MaterialShader shader_type, Blending blending, const UniformSetter& uniform_set) const;
    };
}
