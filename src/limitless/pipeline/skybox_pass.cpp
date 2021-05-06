#include <limitless/pipeline/skybox_pass.hpp>

#include <limitless/skybox/skybox.hpp>
#include <limitless/scene.hpp>

using namespace Limitless;

void SkyboxPass::draw([[maybe_unused]] Instances& instances, Context& ctx, const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] const UniformSetter& setter) {
    skybox.get().draw(ctx, assets);
}

void SkyboxPass::update(Scene& scene, [[maybe_unused]] Instances& instances, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Camera& camera) {
    if (!scene.getSkybox()) {
        throw std::logic_error("There is no skybox in a scene to render!");
    }

    skybox = *scene.getSkybox();
}

SkyboxPass::SkyboxPass(RenderPass* prev, Skybox& _skybox)
    : RenderPass(prev)
    , skybox {_skybox} {
}