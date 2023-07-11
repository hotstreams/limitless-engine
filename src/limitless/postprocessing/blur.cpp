#include <limitless/postprocessing/blur.hpp>

#include <limitless/core/texture/texture_builder.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include "limitless/core/uniform/uniform.hpp"
#include <limitless/assets.hpp>

using namespace Limitless;

void Blur::build(glm::uvec2 frame_size) {
    const uint8_t max_levels = glm::floor(glm::log2(static_cast<float>(glm::max(frame_size.x, frame_size.y)))) + 1;

    ITERATION_COUNT = glm::min(BASE_ITERATION_COUNT, max_levels);

    TextureBuilder builder;
    stage = builder.setTarget(Texture::Type::Tex2D)
            .setFormat(Texture::Format::RGB)
            .setInternalFormat(Texture::InternalFormat::RGB16F)
            .setDataType(Texture::DataType::Float)
            .setSize(frame_size)
            .setWrapS(Texture::Wrap::ClampToEdge)
            .setWrapT(Texture::Wrap::ClampToEdge)
            .setMinFilter(Texture::Filter::LinearMipmapLinear)
            .setMagFilter(Texture::Filter::Linear)
            .setLevels(ITERATION_COUNT)
            .setMipMap(true)
            .build();

    out = builder.setTarget(Texture::Type::Tex2D)
            .setFormat(Texture::Format::RGB)
            .setInternalFormat(Texture::InternalFormat::RGB16F)
            .setDataType(Texture::DataType::Float)
            .setSize(frame_size)
            .setWrapS(Texture::Wrap::ClampToEdge)
            .setWrapT(Texture::Wrap::ClampToEdge)
            .setMinFilter(Texture::Filter::LinearMipmapLinear)
            .setMagFilter(Texture::Filter::Linear)
            .setLevels(ITERATION_COUNT)
            .setMipMap(true)
            .build();

    outRT.clear();
    outRT.resize(ITERATION_COUNT);
    stageRT.clear();
    stageRT.resize(ITERATION_COUNT);
    for (int i = 0; i < ITERATION_COUNT; ++i) {
        outRT[i].bind();
        outRT[i] << TextureAttachment{FramebufferAttachment::Color0, out, 0, static_cast<uint32_t>(i)};
        outRT[i].drawBuffer(FramebufferAttachment::Color0);
        outRT[i].checkStatus();
        outRT[i].unbind();

        stageRT[i].bind();
        stageRT[i] << TextureAttachment{FramebufferAttachment::Color0, stage, 0, static_cast<uint32_t>(i)};
        stageRT[i].drawBuffer(FramebufferAttachment::Color0);
        stageRT[i].checkStatus();
        stageRT[i].unbind();
    }
}

Blur::Blur(glm::uvec2 frame_size) {
    build(frame_size);
}

void Blur::onFramebufferChange(glm::uvec2 frame_size) {
    build(frame_size);
}

void Blur::process(Context& ctx, const Assets& assets, const std::shared_ptr<Texture>& source) {
    for (auto& RT : stageRT) {
        RT.clear();
    }

    for (auto& RT : outRT) {
        RT.clear();
    }

    downsample(ctx, assets, source);
    upsample(ctx, assets);
}

const std::shared_ptr<Texture>& Blur::getResult() const noexcept {
    return out;
}

void Blur::downsample(Context& ctx, const Assets& assets, const std::shared_ptr<Texture>& source) {
    ctx.disable(Capabilities::StencilTest);

    auto& blur = assets.shaders.get("blur_downsample");

    blur .setUniform("source", source)
         .setUniform("level", 0.0f);

    auto vp = ctx.getViewPort();
    for (uint8_t i = 0; i < ITERATION_COUNT; ++i) {
        const auto parity = (i % 2) == 0;

        auto& RT = parity ? outRT[i] : stageRT[i];

        RT.bind();
        ctx.setViewPort(glm::uvec2 {vp.z >> i, vp.w >> i});

        blur.use();

        assets.meshes.at("quad")->draw();

        blur .setUniform("source", parity ? out : stage);
        blur .setUniform("level", static_cast<float>(i));
    }
}

void Blur::upsample(Context& ctx, const Assets& assets) {
    auto& blur = assets.shaders.get("blur_upsample");

    ctx.enable(Capabilities::Blending);
    ctx.setBlendFunc(BlendFactor::One, BlendFactor::One);


    for (uint8_t j = ITERATION_COUNT, i = j - 1; i >= 1; i--, j++) {
        const auto parity = (j % 2) == 0;

        auto& RT = parity ? outRT[i - 1] : stageRT[i - 1];

        auto size = out->getSize();
        auto w = size.x >> (i - 1);
        auto h = size.y >> (i - 1);
        ctx.setViewPort({w, h});

        RT.bind();
        RT.drawBuffer(FramebufferAttachment::Color0);

        //TODO: set nearest filter to sampler ?
//        auto s = parity ? stage : out;
//        s->setMagFilter(Texture::Filter::Linear);
//        s->setMinFilter(Texture::Filter::LinearMipMapNearest);

        blur .setUniform("source", parity ? stage : out)
                .setUniform("resolution", glm::vec4{w, h, 1.0f / w, 1.0f / h})
                .setUniform("level", static_cast<float>(i));

        blur.use();

        assets.meshes.at("quad")->draw();
    }

    ctx.disable(Capabilities::Blending);

    for (size_t i = 1; i < ITERATION_COUNT; i += 2) {
        outRT[i].blit(stageRT[i], Texture::Filter::Nearest);
    }
}

void Blur::setIterationCount(uint8_t count) noexcept {
    BASE_ITERATION_COUNT = count;
    build(out->getSize());
}

uint8_t Blur::getIterationCount() const noexcept {
    return BASE_ITERATION_COUNT;
}
