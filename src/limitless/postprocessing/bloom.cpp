#include <limitless/postprocessing/bloom.hpp>

#include <limitless/assets.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/core/uniform/uniform.hpp>
#include <limitless/renderer/renderer_settings.hpp>

#include <algorithm>
#include <cmath>

using namespace Limitless;

namespace {
    constexpr uint32_t kMaxBloomLevels = 12u;

    glm::uvec2 compute_bloom_plane(glm::uvec2 screen, uint32_t bloom_height) {
        if (screen.x < 2u || screen.y < 2u) {
            return {2u, 2u};
        }
        const float aspect = static_cast<float>(screen.x) / static_cast<float>(screen.y);
        uint32_t h = std::max(2u, bloom_height);
        uint32_t w = std::max(2u, static_cast<uint32_t>(std::floor(static_cast<float>(h) * aspect)));
        return {w, h};
    }
} // namespace

Bloom::Bloom() = default;

bool Bloom::needs_rebuild(const RendererSettings& settings) const noexcept {
    return tex_out == nullptr || bloom_wh_ != compute_bloom_plane(screen_, settings.bloom_buffer_height)
        || cached_bloom_buffer_height_ != settings.bloom_buffer_height
        || cached_bloom_levels_req_ != settings.bloom_blur_iteration_count;
}

void Bloom::rebuild(const RendererSettings& settings) {
    bloom_wh_ = compute_bloom_plane(screen_, settings.bloom_buffer_height);
    const uint32_t major = std::max(bloom_wh_.x, bloom_wh_.y);
    const uint32_t max_mips = 1u + static_cast<uint32_t>(std::floor(std::log2(static_cast<float>(major))));
    levels = std::min(kMaxBloomLevels, std::max(2u, settings.bloom_blur_iteration_count));
    levels = std::min(levels, max_mips);

    cached_bloom_buffer_height_ = settings.bloom_buffer_height;
    cached_bloom_levels_req_ = settings.bloom_blur_iteration_count;

    auto make_chain_tex = [&] {
        return Texture::builder()
            .target(Texture::Type::Tex2D)
            .format(Texture::Format::RGB)
            .internal_format(Texture::InternalFormat::RGB16F)
            .data_type(Texture::DataType::Float)
            .size(bloom_wh_)
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .min_filter(Texture::Filter::LinearMipMapNearest)
            .mag_filter(Texture::Filter::Linear)
            .levels(levels)
            .mipmap(true)
            .build();
    };

    tex_out = make_chain_tex();
    tex_stage = make_chain_tex();

    out_rt.clear();
    out_rt.resize(levels);
    stage_rt.clear();
    stage_rt.resize(levels);
    for (uint32_t i = 0; i < levels; ++i) {
        out_rt[i].bind();
        out_rt[i] << TextureAttachment{FramebufferAttachment::Color0, tex_out, 0, i};
        out_rt[i].drawBuffer(FramebufferAttachment::Color0);
        out_rt[i].checkStatus();
        out_rt[i].unbind();

        stage_rt[i].bind();
        stage_rt[i] << TextureAttachment{FramebufferAttachment::Color0, tex_stage, 0, i};
        stage_rt[i].drawBuffer(FramebufferAttachment::Color0);
        stage_rt[i].checkStatus();
        stage_rt[i].unbind();
    }
}

void Bloom::onFramebufferChange(glm::uvec2 screen_size) {
    screen_ = screen_size;
    tex_out.reset();
    tex_stage.reset();
    out_rt.clear();
    stage_rt.clear();
}

void Bloom::run_prefilter(Context& ctx, const Assets& assets, const std::shared_ptr<Texture>& hdr,
    const RendererSettings& settings) {
    auto& sh = assets.shaders.get("bloom_prefilter");

    const float inv_h =
        (settings.bloom_highlight > 1e-6f) ? (1.0f / settings.bloom_highlight) : 0.0f;

    sh.setUniform("source", hdr)
        .setUniform("threshold", settings.bloom_extract_threshold)
        .setUniform("fireflies", settings.bloom_fireflies_reduction ? 1.0f : 0.0f)
        .setUniform("inv_highlight", inv_h);

    out_rt[0].bind();
    ctx.setViewPort(bloom_wh_);
    out_rt[0].clear();
    sh.use();
    assets.meshes.at("quad")->draw();
    out_rt[0].unbind();
}

void Bloom::run_downsample(Context& ctx, const Assets& assets) {
    ctx.disable(Capabilities::StencilTest);
    auto& sh = assets.shaders.get("bloom_downsample9");

    for (uint32_t i = 1; i < levels; ++i) {
        const bool parity = (i % 2u) == 0u;
        auto& dst = parity ? out_rt[i] : stage_rt[i];
        const std::shared_ptr<Texture>& src_tex = (i % 2u == 1u) ? tex_out : tex_stage;
        const float src_level = static_cast<float>(i - 1u);

        dst.bind();
        ctx.setViewPort({bloom_wh_.x >> i, bloom_wh_.y >> i});
        dst.clear();
        sh.setUniform("source", src_tex).setUniform("source_level", src_level);
        sh.use();
        assets.meshes.at("quad")->draw();
        dst.unbind();
    }
}

void Bloom::run_upsample(Context& ctx, const Assets& assets) {
    auto& sh = assets.shaders.get("blur_upsample");

    ctx.enable(Capabilities::Blending);
    ctx.setBlendFunc(BlendFactor::One, BlendFactor::One);

    for (uint32_t j = levels, i = j - 1u; i >= 1u; --i, ++j) {
        const bool parity = (j % 2u) == 0u;
        auto& dst = parity ? out_rt[i - 1u] : stage_rt[i - 1u];

        const uint32_t w = bloom_wh_.x >> (i - 1u);
        const uint32_t h = bloom_wh_.y >> (i - 1u);
        ctx.setViewPort({w, h});

        dst.bind();
        dst.drawBuffer(FramebufferAttachment::Color0);

        const std::shared_ptr<Texture>& src_tex = parity ? tex_stage : tex_out;
        sh.setUniform("source", src_tex)
            .setUniform("resolution", glm::vec4{static_cast<float>(w), static_cast<float>(h), 1.0f / static_cast<float>(w),
                1.0f / static_cast<float>(h)})
            .setUniform("level", static_cast<float>(i));

        sh.use();
        assets.meshes.at("quad")->draw();
        dst.unbind();
    }

    ctx.disable(Capabilities::Blending);

    for (size_t i = 1; i < levels; i += 2) {
        out_rt[i].blit(stage_rt[i], Texture::Filter::Nearest);
    }
}

void Bloom::process(Context& ctx, const Assets& assets, const std::shared_ptr<Texture>& hdr_image,
    const RendererSettings& settings) {
    if (needs_rebuild(settings)) {
        rebuild(settings);
    }

    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::StencilTest);
    ctx.disable(Capabilities::Blending);

    strength = settings.bloom_strength;

    for (auto& rt : stage_rt) {
        rt.clear();
    }
    for (uint32_t i = 1; i < levels; ++i) {
        out_rt[i].clear();
    }

    run_prefilter(ctx, assets, hdr_image, settings);
    run_downsample(ctx, assets);
    run_upsample(ctx, assets);
}
