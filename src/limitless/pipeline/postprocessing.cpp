#include <limitless/pipeline/postprocessing.hpp>

#include <limitless/models/model.hpp>
#include <limitless/core/texture_builder.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/core/uniform.hpp>

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

    postprocess_shader << UniformValue("bloom", static_cast<int>(bloom))
                       << UniformSampler("bloom_image", bloom_process.getResult())
                       << UniformValue("tone_mapping", static_cast<int>(tone_mapping))
                       << UniformValue("exposure", exposure)
                       << UniformValue("gamma_correction", static_cast<int>(gamma_correction))
                       << UniformValue("gamma", gamma)
                       << UniformValue("vignette", static_cast<int>(vignette))
                       << UniformValue("vignette_radius", vignette_radius)
                       << UniformValue("vignette_softness", vignette_softness)
                       << UniformValue("tone_shading", static_cast<int>(tone_shading))
                       << UniformValue("number_of_colors", number_of_colors)
                       << UniformValue("line_texel_offset", line_texel_offset)
                       << UniformValue("line_multiplier", line_multiplier)
                       << UniformValue("line_bias", line_bias)
                       << UniformValue("fxaa", static_cast<int>(fxaa))
                       << UniformSampler("image", offscreen.get(FramebufferAttachment::Color0).texture)
                       << UniformSampler("image_depth", offscreen.get(FramebufferAttachment::Depth).texture);

    postprocess_shader.use();

    assets.meshes.at("quad")->draw();

    target.unbind();
}

void PostProcessing::onResize(glm::uvec2 frame_size) {
    bloom_process.onFramebufferChange(frame_size);
}
