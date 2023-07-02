#include <limitless/pipeline/deferred/composite_pass.hpp>

#include "limitless/core/uniform/uniform.hpp"
#include <limitless/assets.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include <limitless/pipeline/common/bloom_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred/deferred_framebuffer_pass.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/pipeline/deferred/translucent_pass.hpp>
#include <limitless/pipeline/deferred/deferred_lighting_pass.hpp>

using namespace Limitless;

CompositePass::CompositePass(Pipeline& pipeline, glm::uvec2 size)
    : RenderPass(pipeline)
    , framebuffer {Framebuffer::asRGB8LinearClampToEdge(size)} {
}

std::shared_ptr<Texture> CompositePass::getResult() {
	return framebuffer.get(FramebufferAttachment::Color0).texture;
}

void CompositePass::draw([[maybe_unused]] Instances& instances, Context& ctx, const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    {
        ctx.setViewPort(getResult()->getSize());
        framebuffer.clear();

        auto& shader = assets.shaders.get("composite");

        shader.setUniform("lightened", pipeline.get<TranslucentPass>().getResult());

        {
            auto& bloom_pass = pipeline.get<BloomPass>();
            //TODO: move to bloom
            const auto bloom_strength = bloom_pass.getBloom().strength / static_cast<float>(bloom_pass.getBloom().blur.getIterationCount());
            //TODO: what if there is no bloom ?
            shader.setUniform("bloom", bloom_pass.getResult())
                  .setUniform("bloom_strength", bloom_strength);
        }

        shader.use();

        assets.meshes.at("quad")->draw();
    }
}

void CompositePass::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);
}
