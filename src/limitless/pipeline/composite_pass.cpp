#include <limitless/pipeline/composite_pass.hpp>

#include <limitless/core/uniform.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/pipeline/blur_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred_framebuffer_pass.hpp>
#include <limitless/core/texture_builder.hpp>
#include <limitless/pipeline/translucent_pass.hpp>
#include <limitless/pipeline/deferred_lighting_pass.hpp>

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

        shader << UniformSampler{"lightened", pipeline.get<TranslucentPass>().getResult()};

        {
            auto& bloom_pass = pipeline.get<BloomPass>();
            const auto bloom_strength = bloom_pass.getBloom().strength / static_cast<float>(bloom_pass.getBloom().blur.getIterationCount());

            shader << UniformSampler{"bloom", bloom_pass.getResult()}
                   << UniformValue{"bloom_strength", bloom_strength};
        }

        shader.use();

        assets.meshes.at("quad")->draw();
    }
}

void CompositePass::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);
}
