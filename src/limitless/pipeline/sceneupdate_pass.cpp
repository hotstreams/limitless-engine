#include <limitless/pipeline/sceneupdate_pass.hpp>

#include <limitless/scene.hpp>

using namespace Limitless;

SceneUpdatePass::SceneUpdatePass(Pipeline& pipeline, Context& ctx)
    : RenderPass(pipeline)
    , scene_data {ctx} {
}

void SceneUpdatePass::update(Scene& scene, Instances& instances, Context& ctx, glm::uvec2 frame_size, const Camera& camera) {
    scene.update(ctx, camera);
    scene_data.update(ctx, frame_size, camera);

    instances = scene.getWrappers();
}