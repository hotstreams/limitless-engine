#pragma once

namespace Limitless {
    class Context;
    class Scene;
    class Assets;

    class RendererHelper {
    private:
        static void renderLightsVolume(Context& context, const Scene& scene, const Assets& assets);
        static void renderCoordinateSystemAxes(Context& context, const Assets& assets);
        static void renderBoundingBoxes(Context& context, const Assets& assets, const Scene& scene);
    public:
        static void render(Context& context, const Assets& assets, const Scene& scene);
    };
}