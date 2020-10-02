#pragma once

#include <context_observer.hpp>
#include <memory>
#include <stdexcept>

namespace GraphicsEngine {
    class Texture;

    class RenderTarget {
    protected:
        GLuint id;
    public:
        RenderTarget() noexcept : id(0) { }
        virtual ~RenderTarget() = default;
    };

    enum class FramebufferAttachment {
        Depth = GL_DEPTH_ATTACHMENT,
        Stencil = GL_STENCIL_ATTACHMENT,
        DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT,
        Color0 = GL_COLOR_ATTACHMENT0,
        Color1 = GL_COLOR_ATTACHMENT1,
        Color2 = GL_COLOR_ATTACHMENT2,
        Color3 = GL_COLOR_ATTACHMENT3,
        Color4 = GL_COLOR_ATTACHMENT4,
        Color5 = GL_COLOR_ATTACHMENT5,
        Color6 = GL_COLOR_ATTACHMENT6,
        Color7 = GL_COLOR_ATTACHMENT7
    };

    struct TextureAttachment {
        FramebufferAttachment attachment;
        std::shared_ptr<Texture> texture;
        uint32_t layer {0};
    };

    class incomplete_framebuffer : public std::runtime_error {
    public:
        explicit incomplete_framebuffer(const std::string& error) : runtime_error(error) {}
    };

    class Framebuffer : public RenderTarget, public FramebufferObserver {
    private:
        std::unordered_map<FramebufferAttachment, TextureAttachment> attachments;
    public:
        explicit Framebuffer(ContextEventObserver &context) noexcept;
        Framebuffer() noexcept;
        ~Framebuffer() override;

        Framebuffer(const Framebuffer&) noexcept = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;

        Framebuffer(Framebuffer&&) noexcept = default;
        Framebuffer& operator=(Framebuffer&&) = default;

        const TextureAttachment& get(FramebufferAttachment attachment) const;
        void specifyLayer(FramebufferAttachment attachment, uint32_t layer);
        bool hasAttachment(FramebufferAttachment a) const noexcept;
        void drawBuffers(const std::vector<FramebufferAttachment>& a) noexcept;
        void drawBuffer(FramebufferAttachment a) noexcept;
        void unbind() noexcept;
        void bind() noexcept;
        void clear() noexcept;
        void checkStatus();

        Framebuffer& operator<<(const TextureAttachment &attachment) noexcept;

        void onFramebufferChange(glm::uvec2 size) override;

        static void bindDefault() noexcept;
    };
}
