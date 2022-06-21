#include <limitless/core/texture_builder.hpp>
#include <limitless/pipeline/ssao_pass.hpp>
#include <limitless/instances/abstract_instance.hpp>
#include <limitless/pipeline/shader_pass_types.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/util/sorter.hpp>
#include <limitless/core/context.hpp>
#include <limitless/assets.hpp>
#include <limitless/fx/effect_renderer.hpp>
#include <limitless/core/uniform.hpp>
#include <limitless/core/shader_program.hpp>
#include <random>
#include <iostream>
#include <limitless/camera.hpp>
#include <limitless/pipeline/gbuffer_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred_framebuffer_pass.hpp>
#include <limitless/core/buffer_builder.hpp>

using namespace Limitless;

namespace {
    constexpr auto SSAO_BUFFER_NAME = "SSAO_BUFFER";
}

SSAOPass::SSAOPass(Pipeline& pipeline, ContextEventObserver& ctx)
    : RenderPass(pipeline)
    , framebuffer {} {
    TextureBuilder builder;
    auto ssao = builder     .setTarget(Texture::Type::Tex2D)
                            .setInternalFormat(Texture::InternalFormat::RGB8)
                            .setSize(ctx.getSize())
                            .setMinFilter(Texture::Filter::Nearest)
                            .setMagFilter(Texture::Filter::Nearest)
                            .setWrapS(Texture::Wrap::ClampToEdge)
                            .setWrapT(Texture::Wrap::ClampToEdge)
                            .build();

    auto blurred = builder  .setTarget(Texture::Type::Tex2D)
                            .setInternalFormat(Texture::InternalFormat::RGB8)
                            .setSize(ctx.getSize())
                            .setMinFilter(Texture::Filter::Nearest)
                            .setMagFilter(Texture::Filter::Nearest)
                            .setWrapS(Texture::Wrap::ClampToEdge)
                            .setWrapT(Texture::Wrap::ClampToEdge)
                            .build();

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, ssao}
                << TextureAttachment{FramebufferAttachment::Color1, blurred};
    framebuffer.checkStatus();
    framebuffer.unbind();

    generateNoise();
    generateKernel(ctx);
}

SSAOPass::SSAOPass(Pipeline& pipeline, ContextEventObserver& ctx, glm::uvec2 frame_size)
    : RenderPass(pipeline) {
    TextureBuilder builder;
    auto ssao = builder.setTarget(Texture::Type::Tex2D)
            .setInternalFormat(Texture::InternalFormat::RGB8)
            .setSize(frame_size)
            .setMinFilter(Texture::Filter::Nearest)
            .setMagFilter(Texture::Filter::Nearest)
            .setWrapS(Texture::Wrap::ClampToEdge)
            .setWrapT(Texture::Wrap::ClampToEdge)
            .build();

    auto blurred = builder.setTarget(Texture::Type::Tex2D)
            .setInternalFormat(Texture::InternalFormat::RGB8)
            .setSize(frame_size)
            .setMinFilter(Texture::Filter::Nearest)
            .setMagFilter(Texture::Filter::Nearest)
            .setWrapS(Texture::Wrap::ClampToEdge)
            .setWrapT(Texture::Wrap::ClampToEdge)
            .build();

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, ssao}
                << TextureAttachment{FramebufferAttachment::Color1, blurred};
    framebuffer.checkStatus();
    framebuffer.unbind();

    generateNoise();
    generateKernel(ctx);
}

void SSAOPass::generateNoise() {
    std::uniform_real_distribution<GLfloat> r(0.0, 1.0);
    std::default_random_engine gen(std::random_device{}());
    std::vector<glm::vec3> ssao_noise;

    for (uint32_t i = 0; i < data.NOISE_SIZE * data.NOISE_SIZE; ++i) {
        ssao_noise.emplace_back(r(gen) * 2.0 - 1.0, r(gen) * 2.0 - 1.0, 0.0f);
    }

    TextureBuilder builder;
    noise = builder     .setTarget(Texture::Type::Tex2D)
                        .setInternalFormat(Texture::InternalFormat::RGBA16F)
                        .setSize(glm::uvec2(data.NOISE_SIZE))
                        .setFormat(Texture::Format::RGBA)
                        .setDataType(Texture::DataType::Float)
                        .setMinFilter(Texture::Filter::Nearest)
                        .setMagFilter(Texture::Filter::Nearest)
                        .setWrapS(Texture::Wrap::Repeat)
                        .setWrapT(Texture::Wrap::Repeat)
                        .setData(ssao_noise.data())
                        .build();
}

void SSAOPass::generateKernel(Context& ctx) {
    std::uniform_real_distribution<GLfloat> r(0.0, 1.0);
    std::default_random_engine gen(std::random_device{}());

    for (uint32_t i = 0; i < data.KERNEL_SAMPLES_COUNT; ++i) {
        auto sample = glm::vec4{r(gen) * 2.0 - 1.0, r(gen) * 2.0 - 1.0, r(gen), 1.0f};
        sample = glm::normalize(sample);
        sample *= r(gen);

        auto scale = static_cast<float>(i) / static_cast<float>(data.KERNEL_SAMPLES_COUNT);

        scale = glm::mix(0.1f, 1.0f, scale * scale);
        sample *= scale;

        data.kernel[i] = sample;
    }

    BufferBuilder builder;
    buffer = builder.setTarget(Buffer::Type::ShaderStorage)
                    .setData(&data)
                    .setDataSize(sizeof(SSAOData))
                    .setUsage(Buffer::Usage::StaticDraw)
                    .setAccess(Buffer::MutableAccess::None)
                    .build(SSAO_BUFFER_NAME, ctx);
}

void SSAOPass::draw([[maybe_unused]] Instances& instances, Context& ctx, [[maybe_unused]] const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
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

        buffer->bindBase(ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, SSAO_BUFFER_NAME));

        auto& shader = assets.shaders.get("ssao");
        auto& gbuffer = pipeline.get<DeferredFramebufferPass>();

        shader << UniformSampler{"normal_texture", gbuffer.getNormal()}
               << UniformSampler{"depth_texture", gbuffer.getDepth()}
               << UniformSampler{"noise", noise};

        shader.use();

        assets.meshes.at("quad")->draw();
    }

    {
        framebuffer.drawBuffer(FramebufferAttachment::Color1);

        auto& shader = assets.shaders.get("ssao_blur");

        shader << UniformSampler{"ssao", framebuffer.get(FramebufferAttachment::Color0).texture};

        shader.use();

        assets.meshes.at("quad")->draw();
    }
}

void SSAOPass::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);
}
