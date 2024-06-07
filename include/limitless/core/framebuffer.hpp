#pragma once

#include <limitless/core/context.hpp>
#include <limitless/core/texture/texture.hpp>
#include <memory>
#include <stdexcept>

namespace Limitless {
    enum class FramebufferBlit {
        Color = GL_COLOR_BUFFER_BIT,
        Depth = GL_DEPTH_BUFFER_BIT,
        Stencil = GL_STENCIL_BUFFER_BIT
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
		Color7 = GL_COLOR_ATTACHMENT7,
		None = GL_NONE
	};

    class RenderTarget {
    protected:
        GLuint id {};
    public:
        RenderTarget() = default;
        virtual ~RenderTarget() = default;

        auto getId() { return id; }
        virtual void unbind() noexcept = 0;
        virtual void bind() noexcept = 0;
        virtual void clear() = 0;
	    virtual void clear(FramebufferAttachment attachment) = 0;
	    virtual void onFramebufferChange(glm::uvec2 size) = 0;
    };

    struct TextureAttachment {
        FramebufferAttachment attachment;
        std::shared_ptr<Texture> texture;
        uint32_t layer {};
        uint32_t level {};
    };

    class framebuffer_error : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class Framebuffer : public RenderTarget {
    private:
        std::unordered_map<FramebufferAttachment, TextureAttachment> attachments;
        std::vector<FramebufferAttachment> draw_state;
    public:
        Framebuffer() noexcept;
        ~Framebuffer() override;

        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;

        Framebuffer(Framebuffer&&) noexcept;
        Framebuffer& operator=(Framebuffer&&) noexcept;

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

        void blit(Framebuffer& source, Texture::Filter filter, FramebufferBlit blit = FramebufferBlit::Color);

        void reattach();
        Framebuffer& operator<<(const TextureAttachment &attachment) noexcept;

        void onFramebufferChange(glm::uvec2 size) override;

        static Framebuffer asRGB16FLinearClampToEdge(glm::vec2 size);
        static Framebuffer asRGB8LinearClampToEdge(glm::vec2 size);
        static Framebuffer asRGB8LinearClampToEdgeWithDepth(glm::vec2 size, const std::shared_ptr<Texture>& depth);
        static Framebuffer asRGB8LinearClampToEdgeWithDepth(glm::vec2 size);
        static Framebuffer asRGB16FNearestClampToEdge(glm::uvec2 size);
        static Framebuffer asRGB16FNearestClampToEdgeWithDepth(glm::uvec2 size, const std::shared_ptr<Texture>& depth);
    };

    class DefaultFramebuffer : public RenderTarget {
    public:
        DefaultFramebuffer() = default;
        ~DefaultFramebuffer() override = default;

        void unbind() noexcept override;
        void bind() noexcept override;
        void clear() override;
	    void clear(FramebufferAttachment attachment) override;
        void onFramebufferChange(glm::uvec2 size) override;
    };

    inline DefaultFramebuffer default_framebuffer;
}
