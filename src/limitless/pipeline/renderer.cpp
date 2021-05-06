#include <limitless/pipeline/renderer.hpp>

#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/render_pass.hpp>

using namespace Limitless;

Renderer::Renderer(std::unique_ptr<Pipeline> _pipeline)
    : pipeline {std::move(_pipeline)} {
}

void Renderer::draw(Context& context, const Assets& assets, Scene& scene, Camera& camera) {
    pipeline->draw(context, assets, scene, camera);
}