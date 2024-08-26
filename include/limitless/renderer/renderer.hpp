#pragma once

#include <limitless/renderer/renderer_settings.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <memory>

namespace Limitless::ms {
    enum class Blending;
}

namespace Limitless {
    class Context;
    class Scene;
    class Context;
    class Assets;
    class Camera;

    class Renderer final {
    private:
        RendererSettings settings;
        std::unique_ptr<Pipeline> pipeline;
        InstanceRenderer renderer;
    public:
        Renderer(std::unique_ptr<Pipeline> pipeline, const RendererSettings& settings);
        Renderer(Context& ctx, const RendererSettings& settings);
        explicit Renderer(Context& ctx);

//        const RendererSettings& getRendererSettings() const noexcept { return settings; }
//        void updateRendererSettings();



        void update(Context& ctx, Assets& assets);

        void updatePipeline(Context& ctx);


        void draw(Context& context, const Assets& assets, Scene& scene, Camera& camera);

        class Builder {

        };
    };
}
