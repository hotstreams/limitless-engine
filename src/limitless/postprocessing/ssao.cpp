#include <limitless/postprocessing/ssao.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/assets.hpp>
#include <limitless/camera.hpp>
#include <limitless/renderer/renderer.hpp>

#include <algorithm>
#include <cmath>

using namespace Limitless;

namespace {
    constexpr auto SSAO_BUFFER_NAME = "SSAO_BUFFER";
}

SSAO::Settings SSAO::Settings::filamentQualityPreset(SsaoFilamentQuality q) noexcept {
    Settings s{};
    float samples;
    float turns;
    switch (q) {
        default:
        case SsaoFilamentQuality::Low:
            samples = 7.0f;
            turns = 3.0f;
            break;
        case SsaoFilamentQuality::Medium:
            samples = 11.0f;
            turns = 6.0f;
            break;
        case SsaoFilamentQuality::High:
            samples = 16.0f;
            turns = 7.0f;
            break;
        case SsaoFilamentQuality::Ultra:
            samples = 32.0f;
            turns = 14.0f;
            break;
    }
    s.sample_count = {samples, 1.0f / (samples - 0.5f)};
    s.spiral_turns = turns;
    s.power = 1.0f;
    s.bias = 0.0005f;
    s.min_horizon_angle_rad = 0.0f;
    s.debug_mode = 0;
    return s;
}

SSAO::SSAO(Renderer& renderer) {
    frame_resolution = renderer.getResolution();

    const auto half = glm::uvec2{
        std::max(1u, renderer.getResolution().x / 2u),
        std::max(1u, renderer.getResolution().y / 2u)
    };

    auto ssao = Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(Texture::InternalFormat::RGB8)
            .size(half)
            .min_filter(Texture::Filter::Nearest)
            .mag_filter(Texture::Filter::Linear)
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .build();

    auto blurred = Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(Texture::InternalFormat::RGB8)
            .size(half)
            .min_filter(Texture::Filter::Nearest)
            .mag_filter(Texture::Filter::Linear)
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .build();

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, ssao}
                << TextureAttachment{FramebufferAttachment::Color1, blurred};
    framebuffer.checkStatus();
    framebuffer.unbind();

    buffer = Buffer::builder()
            .target(Buffer::Type::Uniform)
            .data(&settings)
            .size(sizeof(Settings))
            .usage(Buffer::Usage::StaticDraw)
            .access(Buffer::MutableAccess::WriteOrphaning)
            .build(SSAO_BUFFER_NAME, *Context::getCurrentContext());

    const float ao_w = static_cast<float>(std::max(1u, frame_resolution.x / 2u));
    const float ao_h = static_cast<float>(std::max(1u, frame_resolution.y / 2u));
    settings.ssao_resolution = {ao_w, ao_h, 1.0f / ao_w, 1.0f / ao_h};
    buffer->mapData(&settings, sizeof(Settings));
}

void SSAO::draw(Context& ctx, const Assets& assets, const std::shared_ptr<Texture>& depth, const std::shared_ptr<Texture>& normal) {
    {
        ctx.disable(Capabilities::DepthTest);
        ctx.disable(Capabilities::Blending);

        // Render at SSAO buffer resolution
        ctx.setViewPort(framebuffer.get(FramebufferAttachment::Color0).texture->getSize());

        framebuffer.drawBuffers({
                                        FramebufferAttachment::Color0,
                                        FramebufferAttachment::Color1
                                });
        framebuffer.clear();
    }

    {
        framebuffer.drawBuffer(FramebufferAttachment::Color0);

        buffer->bindBase(ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::UniformBuffer, SSAO_BUFFER_NAME));

        auto& shader = assets.shaders.get("ssao");

        shader
            .setUniform("depth_texture", depth)
            .setUniform("normal_texture", normal);

        shader.use();

        assets.meshes.at("quad")->draw();
    }

    {
        framebuffer.drawBuffer(FramebufferAttachment::Color1);

        auto& shader = assets.shaders.get("ssao_blur");



        constexpr size_t kernelArraySize = 16; // limited by bilateralBlur.mat
        auto gaussianKernel =
                [kernelArraySize](float* outKernel, size_t gaussianWidth, float stdDev) -> uint32_t {
                    const size_t gaussianSampleCount = std::min(kernelArraySize, (gaussianWidth + 1u) / 2u);
                    for (size_t i = 0; i < gaussianSampleCount; i++) {
                        float const x = float(i);
                        float const g = std::exp(-(x * x) / (2.0f * stdDev * stdDev));
                        outKernel[i] = g;
                    }
                    return uint32_t(gaussianSampleCount);
                };

//        struct BilateralPassConfig {
//            uint8_t kernelSize = 11;
//            bool bentNormals = false;
//            float standardDeviation = 1.0f;
//            float bilateralThreshold = 0.0625f;
//            float scale = 1.0f;
//        };
        float kGaussianSamples[kernelArraySize];
        uint32_t const kGaussianCount = gaussianKernel(kGaussianSamples, 11, 1.0f);

        shader.setUniform("ssao", framebuffer.get(FramebufferAttachment::Color0).texture)
                .setUniform("axis", glm::vec2{1.0f, 0.0f})
                .setUniform("sample_count", kGaussianCount)
                .setUniform("far_plane_over_edge_distance", 100.0f / 0.0625f);

        for (size_t i = 0; i < kernelArraySize; ++i) {
            shader.setUniform("kernel[" + std::to_string(i) + "]", kGaussianSamples[i]);
        }

        shader.use();

        assets.meshes.at("quad")->draw();
    }

    {
        framebuffer.drawBuffer(FramebufferAttachment::Color0);

        auto& shader = assets.shaders.get("ssao_blur");



        constexpr size_t kernelArraySize = 16; // limited by bilateralBlur.mat
        auto gaussianKernel =
                [kernelArraySize](float* outKernel, size_t gaussianWidth, float stdDev) -> uint32_t {
                    const size_t gaussianSampleCount = std::min(kernelArraySize, (gaussianWidth + 1u) / 2u);
                    for (size_t i = 0; i < gaussianSampleCount; i++) {
                        float const x = float(i);
                        float const g = std::exp(-(x * x) / (2.0f * stdDev * stdDev));
                        outKernel[i] = g;
                    }
                    return uint32_t(gaussianSampleCount);
                };

        float kGaussianSamples[kernelArraySize];
        uint32_t const kGaussianCount = gaussianKernel(kGaussianSamples, 11, 1.0f);

        shader.setUniform("ssao", framebuffer.get(FramebufferAttachment::Color1).texture)
                .setUniform("axis", glm::vec2{0.0f, 1.0f})
                .setUniform("sample_count", kGaussianCount)
                .setUniform("far_plane_over_edge_distance", 100.0f / 0.0625f);

//        for (size_t i = 0; i < kernelArraySize; ++i) {
//            shader.setUniform("kernel[" + std::to_string(i) + "]", kGaussianSamples[i]);
//        }

        shader.use();

        glUniform1fv(glGetUniformLocation(shader.getId(), "kernel"), kGaussianCount, kGaussianSamples);

        assets.meshes.at("quad")->draw();
    }
}

void SSAO::updateSettings(const Camera& camera, const Settings& user) {
    const float samples = std::max(2.0f, user.sample_count.x);
    settings.sample_count = {samples, 1.0f / (samples - 0.5f)};
    settings.spiral_turns = user.spiral_turns;
    settings.power = user.power * 2.0f;
    settings.bias = user.bias;

    const float ao_w = static_cast<float>(std::max(1u, frame_resolution.x / 2u));
    const float ao_h = static_cast<float>(std::max(1u, frame_resolution.y / 2u));
    settings.ssao_resolution = {ao_w, ao_h, 1.0f / ao_w, 1.0f / ao_h};

    const float peak = 0.1f * 0.3f;
    const float intensity = (2.0f * static_cast<float>(M_PI) * peak) * 1.0f;
    settings.intensity = intensity / settings.sample_count.x;

    const float inc = (1.0f / (settings.sample_count.x - 0.5f)) * settings.spiral_turns * 2.0f * static_cast<float>(M_PI);
    settings.angle_inc_cos_sin = {glm::cos(inc), glm::sin(inc)};

    // Filament: projectionScale uses depth/SSAO pass dimensions (same W×H), not full framebuffer.
    // Using full-res here while offsets use ssao_resolution.zw made ssDiskRadius ~2× too large in UV
    // vs depth texel grid → strong moiré / cross-hatch on raw AO.
    const float projectionScale = std::min(
            0.5f * camera.getProjection()[0].x * ao_w,
            0.5f * camera.getProjection()[1].y * ao_h);
    settings.projection_scale_radius = projectionScale * 0.3f;

    settings.inv_radius_squared = 1.0f / (0.3f * 0.3f);
    settings.min_horizon_angle_sine_squared = std::pow(std::sin(user.min_horizon_angle_rad), 2.0f);
    settings.min_horizon_angle_rad = user.min_horizon_angle_rad;

    settings.peak2 = peak * peak;

    constexpr size_t levelCount = 8;
    settings.max_level = uint32_t(levelCount - 1);
    settings.debug_mode = user.debug_mode;
}

void SSAO::onFramebufferChange(glm::uvec2 frame_size) {
    frame_resolution = frame_size;
    const auto half = glm::uvec2{
        std::max(1u, frame_size.x / 2u),
        std::max(1u, frame_size.y / 2u)
    };
    framebuffer.onFramebufferChange(half);
}

void SSAO::update(const Camera& camera, const Settings& user) {
    updateSettings(camera, user);
    buffer->mapData(&settings, sizeof(Settings));
}
