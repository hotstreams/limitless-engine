#include <limitless/pipeline/fxaa_pass.hpp>

#include <limitless/core/context.hpp>
#include <limitless/core/uniform.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/composite_pass.hpp>
#include <limitless/core/texture_builder.hpp>
#include <limitless/pipeline/deferred_framebuffer_pass.hpp>

using namespace Limitless;

FXAAPass::FXAAPass(Pipeline& pipeline, ContextEventObserver& ctx)
    : RenderPass(pipeline)
    , framebuffer {Framebuffer::asRGB8LinearClampToEdge(ctx)} {
}

FXAAPass::FXAAPass(Pipeline& pipeline, glm::uvec2 frame_size)
    : RenderPass(pipeline)
    , framebuffer {Framebuffer::asRGB8LinearClampToEdge(frame_size)} {
}

void FXAAPass::draw([[maybe_unused]] Instances& instances, Context& ctx, const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    {
        framebuffer.clear();
        auto& shader = assets.shaders.get("fxaa");

        shader << UniformSampler{"scene", pipeline.get<DeferredFramebufferPass>().getComposite()};

        shader.use();

        assets.meshes.at("quad")->draw();
    }
}
