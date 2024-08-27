#include <limitless/renderer/sceneupdate_pass.hpp>

#include <limitless/scene.hpp>

using namespace Limitless;

SceneUpdatePass::SceneUpdatePass(Renderer& renderer)
    : RendererPass {renderer}
    , scene_data {renderer} {
}

void SceneUpdatePass::update(Scene &scene, const Camera &camera) {
    scene.update(camera);
    scene_data.update(camera);
}

void SceneUpdatePass::onFramebufferChange(glm::uvec2 size) {
    scene_data.onFramebufferChange(size);
}