#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/pipeline/scene_data.hpp>

namespace Limitless {
    class SceneUpdatePass final : public RenderPass {
    private:
        SceneDataStorage scene_data;
    public:
        SceneUpdatePass(RenderPass* prev, Context& ctx);
        ~SceneUpdatePass() override = default;

        void update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera) override;
    };
}