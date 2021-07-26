#pragma once

#include <limitless/scene.hpp>

namespace Limitless {
    class Context;
    class Lighting;
    class Assets;
    class RenderSettings;

    class RendererHelper {
    private:
        const RenderSettings& settings;
        void renderLightsVolume(Context& context, const Lighting& lighting, const Assets& assets, const Camera& camera);
        void renderCoordinateSystemAxes(Context& context, const Assets& assets);
        void renderBoundingBoxes(Context& context, const Assets& assets, Instances& instances);
    public:
        explicit RendererHelper(const RenderSettings& settings);

        void render(Context& context, const Assets& assets, const Camera& camera, const Lighting& lighting, Instances& instances);
    };
}