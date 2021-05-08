#pragma once

namespace Limitless {
    class Context;
    class Scene;
    class Assets;
    class RenderSettings;

    class RendererHelper {
    private:
        const RenderSettings& settings;
        void renderLightsVolume(Context& context, const Scene& scene, const Assets& assets);
        void renderCoordinateSystemAxes(Context& context, const Assets& assets);
        void renderBoundingBoxes(Context& context, const Assets& assets, const Scene& scene);
    public:
        RendererHelper(const RenderSettings& settings);

        void render(Context& context, const Assets& assets, const Scene& scene);
    };
}