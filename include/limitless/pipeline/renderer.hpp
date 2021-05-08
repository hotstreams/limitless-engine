#pragma once

#include <limitless/pipeline/render_settings.hpp>
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
        RenderSettings settings;
    public:
        explicit Renderer(std::unique_ptr<Pipeline> pipeline, const RenderSettings& settings);
        ~Renderer() = default;

        auto& getSettings() noexcept { return settings; }
        [[nodiscard]] const auto& getSettings() const noexcept { return settings; }

        void update(ContextEventObserver& ctx, Assets& assets, Scene& scene);

        void draw(Context& context, const Assets& assets, Scene& scene, Camera& camera);
    };
}
