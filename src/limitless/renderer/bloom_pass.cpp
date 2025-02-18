#include <limitless/renderer/bloom_pass.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/renderer/translucent_pass.hpp>

using namespace Limitless;

BloomPass::BloomPass(Renderer& renderer)
    : RendererPass {renderer}
    , bloom {renderer.getSettings(), renderer.getResolution()} {
}

void BloomPass::render(
        [[maybe_unused]] InstanceRenderer &instance_renderer,
        [[maybe_unused]] Scene &scene,
        Context &ctx,
        const Assets &assets,
        [[maybe_unused]] const Camera &camera,
        [[maybe_unused]] UniformSetter &setter) {
    GPUProfileScope profile_scope {global_gpu_profiler, "Bloom"};
    bloom.process(ctx, assets, renderer.getPass<TranslucentPass>().getResult());
}

void BloomPass::onFramebufferChange(glm::uvec2 size) {
    bloom.onFramebufferChange(size);
}

std::shared_ptr<Texture> BloomPass::getResult() {
    return bloom.getResult();
}
