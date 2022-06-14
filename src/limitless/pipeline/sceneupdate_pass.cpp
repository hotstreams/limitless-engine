#include <limitless/pipeline/sceneupdate_pass.hpp>

#include <limitless/scene.hpp>

using namespace Limitless;

SceneUpdatePass::SceneUpdatePass(Pipeline& pipeline, Context& ctx)
    : RenderPass(pipeline)
    , scene_data {ctx} {
}

void SceneUpdatePass::update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera) {
    scene.update(ctx, camera);
    scene_data.update(ctx, camera);

    instances = scene.getWrappers();
}

void SceneUpdatePass::onFramebufferChange(glm::uvec2 size) {
    scene_data.onFramebufferChange(size);
}
