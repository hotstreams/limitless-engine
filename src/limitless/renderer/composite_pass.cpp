#include <limitless/renderer/composite_pass.hpp>

#include "limitless/core/uniform/uniform.hpp"
#include <limitless/assets.hpp>
#include <limitless/renderer/bloom_pass.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/renderer/translucent_pass.hpp>
#include <limitless/renderer/outline_pass.hpp>

using namespace Limitless;

CompositePass::CompositePass(Renderer& renderer)
    : RendererPass {renderer}
    , framebuffer {Framebuffer::asRGB8LinearClampToEdge(renderer.getResolution())} {
}

std::shared_ptr<Texture> CompositePass::getResult() {
	return framebuffer.get(FramebufferAttachment::Color0).texture;
}

void CompositePass::render(
        [[maybe_unused]] InstanceRenderer &instance_renderer,
        [[maybe_unused]] Scene &scene,
        Context &ctx,
        const Assets &assets,
        [[maybe_unused]] const Camera &camera,
        [[maybe_unused]] UniformSetter &setter
) {
    
    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    {
        ctx.setViewPort(getResult()->getSize());
        framebuffer.clear();

        auto& shader = assets.shaders.get("composite");

        shader.setUniform("lightened", renderer.getPass<TranslucentPass>().getResult());

        {
            shader.setUniform("outline", renderer.getPass<OutlinePass>().getResult())
                  .setUniform("tone_mapping_exposure", tone_mapping_exposure)
                  .setUniform("gamma", gamma);
        }

        shader.use();

        assets.meshes.at("quad")->draw();
    }
}

void CompositePass::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);
}

CompositeWithBloomPass::CompositeWithBloomPass(Renderer& renderer)
    : CompositePass {renderer} {
}

void CompositeWithBloomPass::render(
        [[maybe_unused]] InstanceRenderer &instance_renderer,
        [[maybe_unused]] Scene &scene,
        Context &ctx,
        const Assets &assets,
        [[maybe_unused]] const Camera &camera,
        [[maybe_unused]] UniformSetter &setter) {

    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    {
        ctx.setViewPort(getResult()->getSize());
        framebuffer.clear();

        auto& shader = assets.shaders.get("composite_with_bloom");

        shader.setUniform("lightened", renderer.getPass<TranslucentPass>().getResult());

        {
            auto& bloom_pass = renderer.getPass<BloomPass>();
            //TODO: move to bloom
            const auto bloom_strength = bloom_pass.getBloom().strength / static_cast<float>(bloom_pass.getBloom().blur.getIterationCount());
            //TODO: what if there is no bloom ?
            shader.setUniform("bloom", bloom_pass.getResult())
                  .setUniform("outline", renderer.getPass<OutlinePass>().getResult())
                  .setUniform("bloom_strength", bloom_strength)
                  .setUniform("tone_mapping_exposure", tone_mapping_exposure)
                  .setUniform("gamma", gamma);
        }

        shader.use();

        assets.meshes.at("quad")->draw();
    }
}
