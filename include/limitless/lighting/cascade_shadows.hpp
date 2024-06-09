#pragma once

#include <limitless/core/framebuffer.hpp>
#include <limitless/renderer/renderer_settings.hpp>

namespace Limitless::fx {
    class EffectRenderer;
}

namespace Limitless {
    class Light;
    class Instance;
    class ShaderProgram;
    class Renderer;
    class Camera;
    class Assets;
    class Scene;

    class ShadowFrustum {
    public:
        glm::vec3 points[8];
        glm::mat4 crop {1.0f};
        float near_distance {};
        float far_distance {};
        float fov {};
        float ratio {};
    };

    using Instances = std::vector<std::reference_wrapper<Instance>>;

    class CascadeShadows final {
    private:
        static constexpr auto SPLIT_WEIGHT {0.75f};

        glm::uvec2 shadow_resolution;
        uint8_t split_count;

        std::unique_ptr<Framebuffer> framebuffer;

        std::vector<ShadowFrustum> frustums;
        std::vector<float> far_bounds {};
        std::shared_ptr<Buffer> light_buffer;
        std::vector<glm::mat4> light_space;

        void initBuffers(Context& context);
        void updateFrustums(Context& ctx, const Camera& camera);
        void updateLightMatrices(const Light& light);
    public:
        explicit CascadeShadows(Context& context, const RendererSettings& settings);
        ~CascadeShadows();

        void update(Context& ctx, const RendererSettings& settings);

        void draw(Instances& instances,
                  const Light& light,
                  Context& ctx, const
                  Assets& assets,
                  const Camera& camera,
                  fx::EffectRenderer* renderer);
        void setUniform(ShaderProgram& sh) const;
        void mapData() const;
    };
}
