#include <limitless/renderer/sceneupdate_pass.hpp>
#include <limitless/core/cpu_profiler.hpp>
#include <limitless/scene.hpp>

using namespace Limitless;

SceneUpdatePass::SceneUpdatePass(Renderer& renderer)
    : RendererPass {renderer}
    , scene_data {renderer} {
}

void SceneUpdatePass::update(Scene &scene, const Camera &camera) {
    CpuProfileScope scope(global_profiler, "SceneUpdatePass::update");
    scene.update(camera);
    scene_data.update(camera);
}

void SceneUpdatePass::onFramebufferChange(glm::uvec2 size) {
    scene_data.onFramebufferChange(size);
}