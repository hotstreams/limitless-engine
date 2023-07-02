#include <limitless/pipeline/forward/postprocessing.hpp>

#include <limitless/models/model.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include "limitless/core/uniform/uniform.hpp"

using namespace Limitless;

PostProcessing::PostProcessing(glm::uvec2 frame_size, RenderTarget& _target)
        : target {_target}
        , bloom_process {frame_size} {
}

void PostProcessing::process(Context& ctx, const Assets& assets, const Framebuffer& offscreen) {
    auto& postprocess_shader = assets.shaders.get("postprocess");

    ctx.disable(Capabilities::DepthTest);
    ctx.setDepthMask(DepthMask::True);
    ctx.disable(Capabilities::Blending);

//    if (bloom) {
//        bloom_process.process(assets, offscreen.get(FramebufferAttachment::Color0).texture);
//    }

    target.clear();

    postprocess_shader.setUniform("bloom", static_cast<int>(bloom))
                        .setUniform("bloom_image", bloom_process.getResult())
                        .setUniform("tone_mapping", static_cast<int>(tone_mapping))
                        .setUniform("exposure", exposure)
                        .setUniform("gamma_correction", static_cast<int>(gamma_correction))
                        .setUniform("gamma", gamma)
                        .setUniform("vignette", static_cast<int>(vignette))
                        .setUniform("vignette_radius", vignette_radius)
                        .setUniform("vignette_softness", vignette_softness)
                        .setUniform("tone_shading", static_cast<int>(tone_shading))
                        .setUniform("number_of_colors", number_of_colors)
                        .setUniform("line_texel_offset", line_texel_offset)
                        .setUniform("line_multiplier", line_multiplier)
                        .setUniform("line_bias", line_bias)
                        .setUniform("fxaa", static_cast<int>(fxaa))
                        .setUniform("image", offscreen.get(FramebufferAttachment::Color0).texture)
                        .setUniform("image_depth", offscreen.get(FramebufferAttachment::Depth).texture);

    postprocess_shader.use();

    assets.meshes.at("quad")->draw();

    target.unbind();
}

void PostProcessing::onResize(glm::uvec2 frame_size) {
    bloom_process.onFramebufferChange(frame_size);
}
