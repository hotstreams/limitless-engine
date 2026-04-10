#pragma once

#include <limitless/scene.hpp>

namespace Limitless {
    class Context;
    class Lighting;
    class Assets;
    class RendererSettings;
    class UniformSetter;
    inline bool lock = false; 
    class RendererHelper {
    private:
        const RendererSettings& settings;
        void renderLightsVolume(Context& context, const Lighting& lighting, const Assets& assets, const Camera& camera, const UniformSetter& setter);
        void renderCoordinateSystemAxes(Context& context, const Assets& assets, const UniformSetter& setter);
        void renderBoundingBoxes(Context& context, const Assets& assets, const Camera& camera, Scene& scene, const UniformSetter& setter);
    public:
        explicit RendererHelper(const RendererSettings& settings);

        void render(Context& context, const Assets& assets, const Camera& camera, const Lighting& lighting, Scene& scene, const UniformSetter& setter);
    };
}