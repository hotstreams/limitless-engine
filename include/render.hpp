#pragma once

#include <postprocessing.hpp>
#include <scene_data.hpp>

namespace GraphicsEngine {
    class Render {
    public:
        PostProcessing postprocess;
    private:
        SceneDataStorage scene_data;
        Framebuffer offscreen;

        void dispatchInstances(Context& context, Scene& scene, MaterialShader shader_type, Blending blending) const;
        void renderLightsVolume(Context& context, Scene& scene) const;
        void initializeOffscreenBuffer(ContextEventObserver& ctx);
    public:
        explicit Render(ContextEventObserver& context) noexcept;

        void draw(Context& context, Scene& scene, Camera& camera);
    };
}
