#include <limitless/renderer/renderer_pass.hpp>

using namespace Limitless;

RendererPass::RendererPass(Renderer& renderer) noexcept
    : renderer {renderer} {
}

void RendererPass::addUniformSetter([[maybe_unused]] UniformSetter& setter) {
}

void RendererPass::update([[maybe_unused]] Scene &scene, [[maybe_unused]] const Camera &camera) {
}

void RendererPass::update([[maybe_unused]] const RendererSettings& settings) {
}

void RendererPass::render([[maybe_unused]] InstanceRenderer& instance_renderer,
                          [[maybe_unused]] Limitless::Scene& scene,
                          [[maybe_unused]] Limitless::Context& ctx,
                          [[maybe_unused]] const Assets& assets,
                          [[maybe_unused]] const Camera& camera,
                          [[maybe_unused]] UniformSetter& setter) {
}

void RendererPass::onFramebufferChange([[maybe_unused]] glm::uvec2 size) {
}