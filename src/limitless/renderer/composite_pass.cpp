#include <limitless/renderer/composite_pass.hpp>

#include "limitless/core/uniform/uniform.hpp"
#include <limitless/assets.hpp>
#include <limitless/renderer/bloom_pass.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/renderer/translucent_pass.hpp>
#include <limitless/core/profiler.hpp>
#include <stdexcept>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>

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
        [[maybe_unused]] UniformSetter &setter) {
    ProfilerScope scope{"CompositePass"};

    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::StencilTest);
    ctx.setDepthMask(DepthMask::False);
    ctx.disable(Capabilities::Blending);
    ctx.setCullFace(CullFace::Back);

    {
        ctx.setViewPort(getResult()->getSize());
        framebuffer.clear();

        auto& shader = assets.shaders.get("composite");
        const auto& rs = renderer.getSettings();
        const auto& pp = rs.postprocess;
        const auto& fog = rs.height_fog;

        shader.setUniform("lightened", renderer.getPass<TranslucentPass>().getResult());

        if (fog.include_in_composite_shader) {
            shader.setUniform("depth_texture", renderer.getPass<DeferredFramebufferPass>().getDepth());
        }

        if (rs.bloom) {
            auto& bloom_pass = renderer.getPass<BloomPass>();
            shader.setUniform("bloom", bloom_pass.getResult())
                  .setUniform("bloom_strength", rs.bloom_strength);
        }

        shader.setUniform("tone_mapping_exposure", pp.exposure);

        if (pp.composite_display_gamma) {
            shader.setUniform("gamma", pp.gamma);
        }
        if (pp.composite_white_balance) {
            shader
                .setUniform("white_balance_temperature", pp.white_balance_temperature)
                .setUniform("white_balance_tint", pp.white_balance_tint);
        }
        if (pp.composite_color_grading) {
            shader
                .setUniform("color_grading_contrast", pp.contrast)
                .setUniform("color_grading_saturation", pp.saturation)
                .setUniform("color_grading_lift", pp.lift)
                .setUniform("color_grading_gamma", pp.grade_gamma)
                .setUniform("color_grading_gain", pp.gain);
        }

        if (fog.include_in_composite_shader) {
            shader
                .setUniform("height_fog_enabled", static_cast<int32_t>(fog.enabled))
                .setUniform("height_fog_color", fog.color)
                .setUniform("height_fog_density", fog.density)
                .setUniform("height_fog_height", fog.height)
                .setUniform("height_fog_falloff", fog.falloff)
                .setUniform("height_fog_start_distance", fog.start_distance);
        }

        if (pp.lut_enabled) {
            if (pp.lut_texture.empty() || !assets.textures.contains(pp.lut_texture)) {
                throw std::runtime_error("Postprocess LUT enabled but texture not found in Assets.textures: " + pp.lut_texture);
            }
            const auto& lut = assets.textures.at(pp.lut_texture);
            shader.setUniform("color_grading_lut_intensity", pp.lut_intensity)
                  .setUniform("color_grading_lut_size", static_cast<float>(lut->getSize().x))
                  .setUniform("color_grading_lut", lut);
        }

        shader.use();

        assets.meshes.at("quad")->draw();
    }
}

void CompositePass::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);
}
