#pragma once

#include <memory>

namespace Limitless::ms {
    enum class Blending;
}

namespace Limitless {
    class Pipeline;
    class ContextEventObserver;
    class Scene;
    class Context;
    class Assets;
    class Camera;

    class Renderer final {
    private:
        std::unique_ptr<Pipeline> pipeline;
    public:
        explicit Renderer(std::unique_ptr<Pipeline> pipeline);
        ~Renderer() = default;

        void draw(Context& context, const Assets& assets, Scene& scene, Camera& camera);
    };
}
