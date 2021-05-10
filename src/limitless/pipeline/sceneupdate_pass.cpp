#include <limitless/pipeline/sceneupdate_pass.hpp>

#include <limitless/scene.hpp>

using namespace Limitless;

SceneUpdatePass::SceneUpdatePass(RenderPass* prev, Context& ctx)
    : RenderPass(prev)
    , scene_data {ctx} {
}

void SceneUpdatePass::update(Scene& scene, [[maybe_unused]] Instances& instances, Context& ctx, const Camera& camera) {
    scene.update(ctx, const_cast<Camera&>(camera));
    scene_data.update(ctx, camera);
}