#pragma once

#include <limitless/core/framebuffer.hpp>
#include <limitless/pipeline/render_settings.hpp>

namespace Limitless {
    struct DirectionalLight;
    class AbstractInstance;
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

    using Instances = std::vector<std::reference_wrapper<AbstractInstance>>;

    class CascadeShadows final {
    private:
        static constexpr glm::uvec2 SHADOW_RESOLUTION = RenderSettings::DIRECTIONAL_SHADOW_RESOLUTION;
        static constexpr auto SPLIT_COUNT = RenderSettings::DIRECTIONAL_SPLIT_COUNT;
        static constexpr auto SPLIT_WEIGHT {0.75f};

        Framebuffer fbo;
        std::array<ShadowFrustum, SPLIT_COUNT> frustums;
        std::array<float, SPLIT_COUNT> far_bounds {};
        std::shared_ptr<Buffer> light_buffer;
        std::vector<glm::mat4> light_space;

        void initBuffers(Context& context);
        void updateFrustums(Context& ctx, const Camera& camera);
        void updateLightMatrices(const DirectionalLight& light);
    public:
        explicit CascadeShadows(Context& context);
        ~CascadeShadows() = default;

        void draw(Instances& instances, const DirectionalLight& light, Context& ctx, const Assets& assets, const Camera& camera);
        void setUniform(ShaderProgram& sh) const;
        void mapData() const;
    };
}
