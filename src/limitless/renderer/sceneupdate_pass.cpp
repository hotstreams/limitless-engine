#include <limitless/renderer/sceneupdate_pass.hpp>
#include <limitless/core/cpu_profiler.hpp>
#include <limitless/scene.hpp>
#include <limitless/renderer/renderer.hpp>

using namespace Limitless;

SceneUpdatePass::SceneUpdatePass(Renderer& renderer)
    : RendererPass {renderer}
    , scene_data {renderer} {
}

void SceneUpdatePass::update(Scene &scene, const Camera &camera) {
    ProfilerScope scope("SceneUpdatePass::update");
    // Keep CPU tiled light culling consistent with renderer settings.
    scene.getLighting().setTiledLightTileSize(renderer.getSettings().light_tile_size);
    scene.update(camera);
    scene_data.update(scene, camera);
}

void SceneUpdatePass::onFramebufferChange(glm::uvec2 size) {
    scene_data.onFramebufferChange(size);
}