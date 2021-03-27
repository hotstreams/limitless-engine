#pragma once

#include <limitless/core/framebuffer.hpp>
#include <limitless/render_settings.hpp>

namespace LimitlessEngine {
    struct DirectionalLight;
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

        void initBuffers();
        void updateFrustums(Context& ctx, Camera& camera);
        void updateLightMatrices(DirectionalLight& light);
    public:
        CascadeShadows();
        ~CascadeShadows() = default;

        void castShadows(Renderer& render, const Assets& assets, Scene& scene, Context& ctx, Camera& camera);
        void setUniform(ShaderProgram& sh) const;
        void mapData() const;
    };
}
