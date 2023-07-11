#pragma once

#include <limitless/renderer/render_settings.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <memory>

namespace Limitless::ms {
    enum class Blending;
}

namespace Limitless {
    class ContextEventObserver;
    class Scene;
    class Context;
    class Assets;
    class Camera;

    class Renderer final {
    private:
        RenderSettings settings;
        std::unique_ptr<Pipeline> pipeline;
    public:
        Renderer(std::unique_ptr<Pipeline> pipeline, const RenderSettings& settings);
        Renderer(ContextEventObserver& ctx, const RenderSettings& settings);
        explicit Renderer(ContextEventObserver& ctx);

        ~Renderer() = default;

        auto& getPipeline() noexcept { return *pipeline; }

        auto& getSettings() noexcept { return settings; }
        [[nodiscard]] const auto& getSettings() const noexcept { return settings; }

        void update(ContextEventObserver& ctx, Assets& assets);
        void updatePipeline(ContextEventObserver& ctx);
        void draw(Context& context, const Assets& assets, Scene& scene, Camera& camera);
    };
}
