#pragma once

#include <limitless/core/context_observer.hpp>
#include <memory>
#include <stdexcept>

namespace Limitless {
    class Texture;

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
		Color7 = GL_COLOR_ATTACHMENT7,
		None = GL_NONE
	};

    class RenderTarget {
    protected:
        GLuint id {};
    public:
        RenderTarget() = default;
        virtual ~RenderTarget() = default;

        virtual void unbind() noexcept = 0;
        virtual void bind() noexcept = 0;
        virtual void clear() = 0;
	    virtual void clear(FramebufferAttachment attachment) = 0;
    };

    struct TextureAttachment {
        FramebufferAttachment attachment;
        std::shared_ptr<Texture> texture;
        uint32_t layer {};
    };

    class framebuffer_error : public std::runtime_error {
    public:
        explicit framebuffer_error(const char* error) : runtime_error{error} {}
    };

    class Framebuffer : public RenderTarget, public FramebufferObserver {
    private:
        std::unordered_map<FramebufferAttachment, TextureAttachment> attachments;
        ContextEventObserver* context {};
        std::vector<FramebufferAttachment> draw_state;
    public:
        explicit Framebuffer(ContextEventObserver& context) noexcept;
        Framebuffer() noexcept;
        ~Framebuffer() override;

        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;

        Framebuffer(Framebuffer&&) = delete;
        Framebuffer& operator=(Framebuffer&&) = delete;

        const TextureAttachment& get(FramebufferAttachment attachment) const;
        void specifyLayer(FramebufferAttachment attachment, uint32_t layer);
        bool hasAttachment(FramebufferAttachment a) const noexcept;
        void drawBuffers(const std::vector<FramebufferAttachment>& a) noexcept;
        void drawBuffer(FramebufferAttachment a) noexcept;
        void readBuffer(FramebufferAttachment a) noexcept;
        void unbind() noexcept override;
        void bind() noexcept override;
        void clear() noexcept override;
        void clear(FramebufferAttachment attachment) override;
        void checkStatus();

        Framebuffer& operator<<(const TextureAttachment &attachment) noexcept;

        void onFramebufferChange(glm::uvec2 size) override;
    };

    class DefaultFramebuffer : public RenderTarget {
    public:
        DefaultFramebuffer() = default;
        ~DefaultFramebuffer() override = default;

        void unbind() noexcept override;
        void bind() noexcept override;
        void clear() override;
	    void clear(FramebufferAttachment attachment) override;
    };

    inline DefaultFramebuffer default_framebuffer;
}
