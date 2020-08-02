#pragma once

#include <framebuffer.hpp>
#include <shader_storage.hpp>
#include <elementary_model.hpp>
#include "texture_builder.hpp"

namespace GraphicsEngine {
    class Bloom {
    private:
        static constexpr uint8_t blur_iterations = 8;

        Framebuffer brightness;
        std::array<Framebuffer, 2> blur;
        Quad screen;

        void extractBrightness(const std::shared_ptr<Texture>& image) {
            auto& brightness_shader = *shader_storage.get("brightness");

            brightness.bind();

            brightness_shader << UniformSampler("image", image);

            brightness_shader.use();

            screen.getMesh()->draw();
        }

        void blurImage() {
            auto& blur_shader = *shader_storage.get("blur");

            for (uint8_t i = 0; i < blur_iterations; ++i) {
                auto index = i % 2;
                auto direction = index ? glm::vec2{1.0f, 0.0f} : glm::vec2{0.0f, 1.0f};
                auto image = (i == 0) ? brightness.get(FramebufferAttachment::Color0).texture : blur[!index].get(FramebufferAttachment::Color0).texture;

                blur[index].bind();

                blur_shader << UniformValue<glm::vec2>{"direction", direction}
                            << UniformSampler("image", image);

                blur_shader.use();

                screen.getMesh()->draw();
            }
        }
    public:
        explicit Bloom(ContextEventObserver& ctx) :
            brightness(ctx), blur{ Framebuffer(ctx), Framebuffer(ctx) } {
            auto texture = TextureBuilder::build(Texture::Type::Tex2D, 1, Texture::InternalFormat::RGB16F, ctx.getSize(), Texture::Format::RGB, Texture::DataType::Float, nullptr);

//            *texture << TexParameter<GLint>{GL_TEXTURE_MAG_FILTER, GL_LINEAR}
//                     << TexParameter<GLint>{GL_TEXTURE_MIN_FILTER, GL_LINEAR};

            brightness << TextureAttachment{FramebufferAttachment::Color0, texture};
            brightness.drawBuffer(FramebufferAttachment::Color0);
            brightness.checkStatus();

            for (auto& fbo : blur) {
                auto tex = TextureBuilder::build(Texture::Type::Tex2D, 1, Texture::InternalFormat::RGB16F, ctx.getSize(), Texture::Format::RGB, Texture::DataType::Float, nullptr);

//                *tex << TexParameter<GLint>{GL_TEXTURE_MAG_FILTER, GL_LINEAR}
//                     << TexParameter<GLint>{GL_TEXTURE_MIN_FILTER, GL_LINEAR};

                fbo << TextureAttachment{FramebufferAttachment::Color0, texture};
                fbo.drawBuffer(FramebufferAttachment::Color0);
                fbo.checkStatus();

                fbo.unbind();
            }
        }

        const auto& getResult() const noexcept { return blur[(blur_iterations - 1) % 2].get(FramebufferAttachment::Color0).texture; }

        void process(const std::shared_ptr<Texture>& image) {
            extractBrightness(image);
            blurImage();
        }
    };

    class PostProcessing {
    public:
        bool bloom {true};

        bool tone_mapping {true};
        float exposure {1.0f};

        bool gamma_correction {false};
        float gamma {2.2f};
    private:
        Bloom bloom_process;
        Quad screen;
    public:
        explicit PostProcessing(ContextEventObserver& ctx) : bloom_process(ctx) {

        }

        void process(const std::shared_ptr<Texture>& image) {
            auto& postprocess_shader = *shader_storage.get("postprocess");

            if (bloom) {
                bloom_process.process(image);
            }

            Framebuffer::bindDefault();

            postprocess_shader << UniformValue("bloom", static_cast<int>(bloom))
                               << UniformSampler("bloom_image", bloom_process.getResult())
                               << UniformValue("tone_mapping", static_cast<int>(tone_mapping))
                               << UniformValue("exposure", exposure)
                               << UniformValue("gamma_correction", static_cast<int>(gamma_correction))
                               << UniformValue("gamma", gamma)
                               << UniformSampler("image", image);

            postprocess_shader.use();

            screen.getMesh()->draw();
        }
    };
}