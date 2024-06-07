#include <limitless/core/texture/named_texture.hpp>
#include <limitless/core/context.hpp>
#include <algorithm>

using namespace Limitless;

NamedTexture::NamedTexture(GLenum _target) noexcept : target{_target} {
}

void NamedTexture::generateId() noexcept {
    glCreateTextures(target, 1, &id);
}

NamedTexture::~NamedTexture() {
    if (id != 0) {
        Context::apply([this] (Context& ctx) {
            auto& target_map = ctx.texture_bound;

            std::for_each(target_map.begin(), target_map.end(), [&] (auto& state) {
                auto& [s_unit, s_id] = state;
                if (s_id == id) s_id = 0;
            });

            glDeleteTextures(1, &id);
        });
    }
}

void NamedTexture::texStorage2D([[maybe_unused]] GLenum _target, GLsizei levels, GLenum internal_format, glm::uvec2 size) noexcept {
    glTextureStorage2D(id, levels, internal_format, size.x, size.y);
}

void NamedTexture::texStorage3D([[maybe_unused]] GLenum _target, GLsizei levels, GLenum internal_format, glm::uvec3 size) noexcept {
    glTextureStorage3D(id, levels, internal_format, size.x, size.y, size.z);
}

void NamedTexture::texSubImage2D([[maybe_unused]] GLenum _target, GLint level, GLint xoffset, GLint yoffset, glm::uvec2 size, GLenum format, GLenum type, const void* data) noexcept {
    glTextureSubImage2D(id, level, xoffset, yoffset, size.x, size.y, format, type, data);
}

void NamedTexture::texSubImage3D([[maybe_unused]] GLenum _target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, GLenum format, GLenum type, const void* data) noexcept {
    glTextureSubImage3D(id, level, xoffset, yoffset, zoffset, size.x, size.y, size.z, format, type, data);
}

void NamedTexture::bind([[maybe_unused]] GLenum _target, GLuint index) const {
    Context::apply([this, &index] (Context& ctx) {
        if (index >= static_cast<GLuint>(ContextInitializer::limits.max_texture_units)) {
            throw std::logic_error {"Failed to bind texture to unit greater than accessible"};
        }

        if (ctx.texture_bound[index] != id) {
            glBindTextureUnit(index, id);
            ctx.texture_bound[index] = id;
        }
    });
}

void NamedTexture::generateMipMap([[maybe_unused]] GLenum _target) noexcept {
    glGenerateTextureMipmap(id);
}

NamedTexture* NamedTexture::clone() const {
    return new NamedTexture(target);
}

void NamedTexture::accept(TextureVisitor& visitor) noexcept {
    visitor.visit(*this);
}

NamedTexture::NamedTexture(NamedTexture&& rhs) noexcept {
    std::swap(id, rhs.id);
    std::swap(target, rhs.target);
}

NamedTexture& NamedTexture::operator=(NamedTexture&& rhs) noexcept {
    std::swap(id, rhs.id);
    std::swap(target, rhs.target);
    return *this;
}

NamedTexture& NamedTexture::setMinFilter([[maybe_unused]] GLenum _target, GLenum filter) {
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, filter);
    return *this;
}

NamedTexture& NamedTexture::setMagFilter([[maybe_unused]] GLenum _target, GLenum filter) {
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, filter);
    return *this;
}

NamedTexture& NamedTexture::setAnisotropicFilter([[maybe_unused]] GLenum _target, GLfloat value) {
    glTextureParameterf(id, GL_TEXTURE_MAX_ANISOTROPY, value);
    return *this;
}

NamedTexture& NamedTexture::setBorderColor([[maybe_unused]] GLenum _target, float* color) {
    glTextureParameterfv(id, GL_TEXTURE_BORDER_COLOR, color);
    return *this;
}

NamedTexture& NamedTexture::setWrapS([[maybe_unused]] GLenum _target, GLenum wrap) {
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, wrap);
    return *this;
}

NamedTexture& NamedTexture::setWrapT([[maybe_unused]] GLenum _target, GLenum wrap) {
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, wrap);
    return *this;
}

NamedTexture& NamedTexture::setWrapR([[maybe_unused]] GLenum _target, GLenum wrap) {
    glTextureParameteri(id, GL_TEXTURE_WRAP_R, wrap);
    return *this;
}
