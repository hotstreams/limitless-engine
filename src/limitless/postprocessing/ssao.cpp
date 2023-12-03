#include <limitless/postprocessing/ssao.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/assets.hpp>
#include <limitless/camera.hpp>

using namespace Limitless;

namespace {
    constexpr auto SSAO_BUFFER_NAME = "SSAO_BUFFER";
}

SSAO::SSAO(ContextEventObserver& ctx) {
    auto ssao = Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(Texture::InternalFormat::RGB8)
            .size(ctx.getSize())
            .min_filter(Texture::Filter::Nearest)
            .mag_filter(Texture::Filter::Nearest)
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .build();

    auto blurred = Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(Texture::InternalFormat::RGB8)
            .size(ctx.getSize())
            .min_filter(Texture::Filter::Nearest)
            .mag_filter(Texture::Filter::Nearest)
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
            .build(SSAO_BUFFER_NAME, ctx);
}

SSAO::SSAO(ContextEventObserver &ctx, glm::uvec2 frame_size) {
    auto ssao = Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(Texture::InternalFormat::RGB8)
            .size(frame_size)
            .min_filter(Texture::Filter::Nearest)
            .mag_filter(Texture::Filter::Nearest)
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .build();

    auto blurred = Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(Texture::InternalFormat::RGB8)
            .size(frame_size)
            .min_filter(Texture::Filter::Nearest)
            .mag_filter(Texture::Filter::Nearest)
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
            .build(SSAO_BUFFER_NAME, ctx);
}

void SSAO::draw(Context& ctx, const Assets& assets, const std::shared_ptr<Texture>& depth) {
    {
        ctx.disable(Capabilities::DepthTest);
        ctx.disable(Capabilities::Blending);

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

        shader.setUniform("depth_texture", depth);

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
                .setUniform("far_plane_over_edge_distance", -100.0f / 0.0625f);

//        for (size_t i = 0; i < kernelArraySize; ++i) {
//            shader.setUniform("kernel[" + std::to_string(i) + "]", kGaussianSamples[i]);
//        }

        shader.use();

        glUniform1fv(glGetUniformLocation(shader.getId(), "kernel"), kGaussianCount, kGaussianSamples);

        assets.meshes.at("quad")->draw();
    }
}

void SSAO::updateSettings(const Camera& camera) {
    settings.sample_count = { 7.0f, 1.0f / (7.0f - 0.5f) };
    settings.spiral_turns = 14.0f;
    settings.power = 1.0f;
    settings.bias = 0.0005f;

    const float peak = 0.1f * 0.3f;
    const float intensity = (2.0f * M_PI * peak) * 1.0f;
    settings.intensity = intensity / settings.sample_count.x;

    const float inc = (1.0f / (settings.sample_count.x - 0.5f)) * settings.spiral_turns * 2.0f * M_PI;
    settings.angle_inc_cos_sin = { glm::cos(inc), glm::sin(inc) };

    const float projectionScale = std::min(
            0.5f * camera.getProjection()[0].x * 1080,
            0.5f * camera.getProjection()[1].y * 720);
    settings.projection_scale_radius = projectionScale * 0.3f;

    settings.inv_radius_squared = 1.0f / (0.3f * 0.3f);
    settings.min_horizon_angle_sine_squared = std::pow(std::sin(0.0f), 2.0f);

    settings.peak2 = peak * peak;

    const size_t levelCount = 8;
    settings.max_level = uint32_t(levelCount - 1);
}

void SSAO::onFramebufferChange(glm::uvec2 frame_size) {
    framebuffer.onFramebufferChange(frame_size);
}

void SSAO::update(const Camera &camera) {
    updateSettings(camera);
    buffer->mapData(&settings, sizeof(Settings));
}
