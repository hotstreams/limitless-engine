#pragma once

#include <limitless/pipeline/render_pass.hpp>

namespace Limitless {
    class Skybox;

    class SkyboxPass final : public RenderPass {
    private:
        std::reference_wrapper<Skybox> skybox;
    public:
        explicit SkyboxPass(RenderPass* prev, Skybox& skybox);
        ~SkyboxPass() override = default;

        void update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera) override;

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, const UniformSetter& setter) override;
    };
}