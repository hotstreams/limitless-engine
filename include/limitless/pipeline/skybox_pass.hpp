#pragma once

#include <limitless/pipeline/render_pass.hpp>

namespace Limitless {
    class Skybox;

    class SkyboxPass final : public RenderPass {
    private:
        Skybox* skybox {};
    public:
        explicit SkyboxPass(Pipeline& pipeline);

        void update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera) override;
        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}