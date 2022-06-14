#include <limitless/core/framebuffer.hpp>
#include <limitless/core/texture.hpp>
#include <limitless/core/texture_builder.hpp>

using namespace Limitless;

Framebuffer::Framebuffer() noexcept {
    glGenFramebuffers(1, &id);
}

void Framebuffer::bind() noexcept {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        if (state->framebuffer_id != id) {
            glBindFramebuffer(GL_FRAMEBUFFER, id);
            state->framebuffer_id = id;
        }
    }
}

Framebuffer::~Framebuffer() {
    if (id != 0) {
        if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
            if (state->framebuffer_id == id) {
                state->framebuffer_id = 0;
            }
            glDeleteFramebuffers(1, &id);
        }
    }
}

void Framebuffer::onFramebufferChange(glm::uvec2 size) {
    for (auto& [type, attachment] : attachments) {
        attachment.texture->resize({ size, attachment.texture->getSize().z });
        *this << attachment;
    }
}

void Framebuffer::checkStatus() {
    bind();

    if (auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE) {
        throw framebuffer_error{"Framebuffer is incomplete"};
    }
}

void Framebuffer::clear() noexcept {
    bind();

    GLenum bits = 0;

    if (hasAttachment(FramebufferAttachment::Color0)) {
        bits |= GL_COLOR_BUFFER_BIT;
    }

    if (hasAttachment(FramebufferAttachment::Depth)) {
        bits |= GL_DEPTH_BUFFER_BIT;
    }

    if (hasAttachment(FramebufferAttachment::Stencil)) {
        bits |= GL_STENCIL_BUFFER_BIT;
    }

    if (hasAttachment(FramebufferAttachment::DepthStencil)) {
        bits |= GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
    }

    glClear(bits);
}

void Framebuffer::clear(FramebufferAttachment attachment) {
	bind();

	GLenum bits = 0;

	if (attachment == FramebufferAttachment::Color0) {
		bits |= GL_COLOR_BUFFER_BIT;
	}

	if (attachment == FramebufferAttachment::Depth) {
		bits |= GL_DEPTH_BUFFER_BIT;
	}

	if (attachment == FramebufferAttachment::Stencil) {
		bits |= GL_STENCIL_BUFFER_BIT;
	}

	if (attachment == FramebufferAttachment::DepthStencil) {
		bits |= GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
	}

	glClear(bits);
}

bool Framebuffer::hasAttachment(FramebufferAttachment a) const noexcept {
    return attachments.find(a) != attachments.end();
}

void Framebuffer::drawBuffers(const std::vector<FramebufferAttachment>& a) noexcept {
    bind();

    if (draw_state != a) {
        draw_state = a;

        glDrawBuffers(a.size(), reinterpret_cast<const GLenum*>(a.data()));
    }
}

void Framebuffer::drawBuffer(FramebufferAttachment a) noexcept {
    bind();

    if (draw_state.size() != 1 || draw_state.front() != a) {
        draw_state.clear();
        draw_state.emplace_back(a);

        glDrawBuffer(static_cast<GLenum>(a));
    }
}

void Framebuffer::readBuffer(FramebufferAttachment a) noexcept {
    bind();

    glReadBuffer(static_cast<GLenum>(a));
}

void Framebuffer::specifyLayer(FramebufferAttachment attachment, uint32_t layer) {
    try {
        auto& tex_attachment = attachments.at(attachment);

        if (tex_attachment.layer != layer) {
            bind();
            glFramebufferTextureLayer(GL_FRAMEBUFFER, static_cast<GLenum>(tex_attachment.attachment), tex_attachment.texture->getId(), 0, layer);
            tex_attachment.layer = layer;
        }
    } catch (...) {
        throw framebuffer_error{"No such framebuffer attachment"};
    }
}

Framebuffer& Framebuffer::operator<<(const TextureAttachment& attachment) noexcept {
    bind();

    switch (attachment.texture->getType()) {
        case Texture::Type::Tex2D:
            glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<GLenum>(attachment.attachment), static_cast<GLenum>(attachment.texture->getType()), attachment.texture->getId(), 0);
            break;
        case Texture::Type::CubeMap:
            glFramebufferTexture(GL_FRAMEBUFFER, static_cast<GLenum>(attachment.attachment), attachment.texture->getId(), 0);
            break;
        case Texture::Type::Tex2DArray:
        case Texture::Type::TexCubeMapArray:
        case Texture::Type::Tex3D:
            glFramebufferTextureLayer(GL_FRAMEBUFFER, static_cast<GLenum>(attachment.attachment), attachment.texture->getId(), 0, attachment.layer);
            break;
    }

    attachments[attachment.attachment] = attachment;
    return *this;
}

const TextureAttachment& Framebuffer::get(FramebufferAttachment attachment) const {
    try {
        return attachments.at(attachment);
    } catch (...) {
        throw framebuffer_error{"No such framebuffer attachment"};
    }
}

void Framebuffer::unbind() noexcept {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        if (state->framebuffer_id != 0) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            state->framebuffer_id = 0;
        }
    }
}

Framebuffer Framebuffer::asRGB8LinearClampToEdge(glm::vec2 size) {
    Framebuffer framebuffer;

    TextureBuilder builder;
    auto color = builder.setTarget(Texture::Type::Tex2D)
            .setInternalFormat(Texture::InternalFormat::RGB8)
            .setSize(size)
            .setFormat(Texture::Format::RGB)
            .setDataType(Texture::DataType::UnsignedByte)
            .setMinFilter(Texture::Filter::Linear)
            .setMagFilter(Texture::Filter::Linear)
            .setWrapS(Texture::Wrap::ClampToEdge)
            .setWrapT(Texture::Wrap::ClampToEdge)
            .build();

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, color};
    framebuffer.checkStatus();
    framebuffer.unbind();

    return framebuffer;
}

Framebuffer Framebuffer::asRGB8LinearClampToEdgeWithDepth(glm::vec2 size, const std::shared_ptr<Texture>& depth) {
    Framebuffer framebuffer;

    TextureBuilder builder;
    auto color = builder.setTarget(Texture::Type::Tex2D)
            .setInternalFormat(Texture::InternalFormat::RGB8)
            .setSize(size)
            .setFormat(Texture::Format::RGB)
            .setDataType(Texture::DataType::UnsignedByte)
            .setMinFilter(Texture::Filter::Linear)
            .setMagFilter(Texture::Filter::Linear)
            .setWrapS(Texture::Wrap::ClampToEdge)
            .setWrapT(Texture::Wrap::ClampToEdge)
            .build();

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, color}
                << TextureAttachment{FramebufferAttachment::Depth, depth};
    framebuffer.checkStatus();
    framebuffer.unbind();

    return framebuffer;
}

Framebuffer Framebuffer::asRGB8LinearClampToEdgeWithDepth(glm::vec2 size) {
    Framebuffer framebuffer;

    TextureBuilder builder;
    auto color = builder.setTarget(Texture::Type::Tex2D)
            .setInternalFormat(Texture::InternalFormat::RGB8)
            .setSize(size)
            .setFormat(Texture::Format::RGB)
            .setDataType(Texture::DataType::UnsignedByte)
            .setMinFilter(Texture::Filter::Linear)
            .setMagFilter(Texture::Filter::Linear)
            .setWrapS(Texture::Wrap::ClampToEdge)
            .setWrapT(Texture::Wrap::ClampToEdge)
            .build();

    //TODO: 32F???? wtf?
    auto depth = builder.setTarget(Texture::Type::Tex2D)
            .setInternalFormat(Texture::InternalFormat::Depth32F)
            .setSize(size)
            .setFormat(Texture::Format::DepthComponent)
            .setDataType(Texture::DataType::Float)
            .setMinFilter(Texture::Filter::Linear)
            .setMagFilter(Texture::Filter::Linear)
            .setWrapS(Texture::Wrap::ClampToEdge)
            .setWrapT(Texture::Wrap::ClampToEdge)
            .build();

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, color}
                << TextureAttachment{FramebufferAttachment::Depth, depth};
    framebuffer.checkStatus();
    framebuffer.unbind();

    return framebuffer;
}

Framebuffer Framebuffer::asRGB16FLinearClampToEdge(glm::vec2 size) {
    Framebuffer framebuffer;

    TextureBuilder builder;
    auto color = builder.setTarget(Texture::Type::Tex2D)
            .setInternalFormat(Texture::InternalFormat::RGB16F)
            .setSize(size)
            .setFormat(Texture::Format::RGB)
            .setDataType(Texture::DataType::Float)
            .setMinFilter(Texture::Filter::Linear)
            .setMagFilter(Texture::Filter::Linear)
            .setWrapS(Texture::Wrap::ClampToEdge)
            .setWrapT(Texture::Wrap::ClampToEdge)
            .build();

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, color};
    framebuffer.drawBuffer(FramebufferAttachment::Color0);
    framebuffer.checkStatus();
    framebuffer.unbind();

    return framebuffer;
}

Framebuffer::Framebuffer(Framebuffer&& rhs) noexcept
    : attachments {std::move(rhs.attachments)}
    , draw_state {std::move(rhs.draw_state)} {
}
//TODO: tests
// remove rhs from observers?
Framebuffer& Framebuffer::operator=(Framebuffer&& rhs) noexcept {
    attachments = std::move(rhs.attachments);
    draw_state = std::move(rhs.draw_state);
    return *this;
}

void DefaultFramebuffer::unbind() noexcept {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        if (state->framebuffer_id != 0) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            state->framebuffer_id = 0;
        }
    }
}

void DefaultFramebuffer::bind() noexcept {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        if (state->framebuffer_id != 0) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            state->framebuffer_id = 0;
        }
    }
}

void DefaultFramebuffer::clear() {
    bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void DefaultFramebuffer::clear(FramebufferAttachment attachment) {
    bind();

    GLenum bits = 0;

    if (attachment == FramebufferAttachment::Color0) {
        bits |= GL_COLOR_BUFFER_BIT;
    }

    if (attachment == FramebufferAttachment::Depth) {
        bits |= GL_DEPTH_BUFFER_BIT;
    }

    if (attachment == FramebufferAttachment::Stencil) {
        bits |= GL_STENCIL_BUFFER_BIT;
    }

    if (attachment == FramebufferAttachment::DepthStencil) {
        bits |= GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
    }

    glClear(bits);
}

void DefaultFramebuffer::onFramebufferChange(glm::uvec2 size) {
    // nothing, its done automatically
}
