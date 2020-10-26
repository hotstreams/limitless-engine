#include <named_texture.hpp>
#include <context_state.hpp>
#include <stdexcept>

using namespace GraphicsEngine;

void NamedTexture::texStorage2D(GLsizei levels, InternalFormat internal) const noexcept {
    glTextureStorage2D(id, levels, static_cast<GLenum>(internal), size.x, size.y);
}

void NamedTexture::texStorage3D(GLsizei levels, InternalFormat internal, glm::uvec3 _size) const noexcept {
    glTextureStorage3D(id, levels, static_cast<GLenum>(internal), _size.x, _size.y, _size.z);
}

void NamedTexture::texStorage2DMultisample(uint8_t samples, InternalFormat internal) const noexcept {
    glTextureStorage2DMultisample(id, samples, static_cast<GLenum>(internal), size.x, size.y, GL_FALSE);
}

void NamedTexture::texSubImage2D(GLint xoffset, GLint yoffset, glm::uvec2 _size, const void* data) const noexcept {
    glTextureSubImage2D(id, 0, xoffset, yoffset, _size.x, _size.y, static_cast<GLenum>(format), static_cast<GLenum>(data_type), data);
}

void NamedTexture::texSubImage3D(GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 _size, const void *data) const noexcept {
    glTextureSubImage3D(id, 0, xoffset, yoffset, zoffset, _size.x, _size.y, _size.z, static_cast<GLenum>(format), static_cast<GLenum>(data_type), data);
}

void NamedTexture::generateMipMap() const noexcept {
    glGenerateTextureMipmap(id);
}

void NamedTexture::bind(GLuint index) const noexcept {
    auto* window = glfwGetCurrentContext();
    if (ContextState::hasState(window)) {
        auto &bound = ContextState::getState(window)->texture_bound[index];
        if (bound != id) {
            glBindTextureUnit(index, id);
            bound = id;
        }
    }
}

NamedTexture& NamedTexture::operator<<(const TexParameter<GLint>& param) noexcept {
    glTextureParameteri(id, param.name, param.param);
    return *this;
}

NamedTexture& NamedTexture::operator<<(const TexParameter<GLfloat>& param) noexcept {
    glTextureParameterf(id, param.name, param.param);
    return *this;
}

NamedTexture& NamedTexture::operator<<(const TexParameter<GLint*>& param) noexcept {
    glTextureParameteriv(id, param.name, param.param);
    return *this;
}

NamedTexture& NamedTexture::operator<<(const TexParameter<GLfloat*>& param) noexcept {
    glTextureParameterfv(id, param.name, param.param);
    return *this;
}

NamedTexture::NamedTexture(Type target, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const void* data, const texture_parameters& params)
        : StateTexture(0, target, { size.x, size.y, 0 }, internal, format, data_type) {
    glCreateTextures(static_cast<GLenum>(target), 1, &id);

    params(*this);

    switch (target) {
        case Type::Tex2D:
            texImage2D(static_cast<GLenum>(target), internal, data);
            break;
        case Type::Tex2DArray:
        case Type::Tex3D:
        case Type::CubeMap:
        case Type::TexCubeMapArray:
        case Type::Tex2DMS:
            glDeleteTextures(1, &id);
            throw std::logic_error("Wrong constructor usage to build texture.");
    }
}

NamedTexture::NamedTexture(Type target, InternalFormat internal, glm::uvec3 size, Format format, DataType data_type, const void* data, const texture_parameters& params)
        : StateTexture(0, target, size, internal, format, data_type) {
    glCreateTextures(static_cast<GLenum>(target), 1, &id);

    params(*this);

    switch (target) {
        case Type::Tex2DArray:
            [[fallthrough]];
        case Type::Tex3D:
            texImage3D(static_cast<GLenum>(target), internal, size, data);
            break;
        case Type::TexCubeMapArray:
            texImage3D(static_cast<GLenum>(target), internal, { size.x, size.y, size.z * 6 }, nullptr);
            break;
        case Type::Tex2D:
        case Type::CubeMap:
        case Type::Tex2DMS:
            glDeleteTextures(1, &id);
            throw std::logic_error("Wrong constructor usage to build texture.");
    }
}

NamedTexture::NamedTexture(Type target, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params)
        : StateTexture(0, target, { size.x, size.y, 0 }, internal, format, data_type) {
    glCreateTextures(static_cast<GLenum>(target), 1, &id);

    params(*this);

    switch (target) {
        case Type::CubeMap:
            for (int i = 0; i < 6; ++i) {
                texImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, internal, data[i]);
            }
            break;
        case Type::Tex2D:
        case Type::Tex3D:
        case Type::Tex2DArray:
        case Type::TexCubeMapArray:
        case Type::Tex2DMS:
            glDeleteTextures(1, &id);
            throw std::logic_error("Wrong constructor usage to build texture.");
    }
}

NamedTexture::NamedTexture(Type target, uint8_t samples, InternalFormat internal, glm::uvec2 size, bool immutable, const texture_parameters& params)
        : StateTexture(0, target, { size.x, size.y, 0 }, internal, Format::RGB, DataType::Int) {
    glCreateTextures(static_cast<GLenum>(target), 1, &id);

    params(*this);

    switch (target) {
        case Type::Tex2DMS:
            immutable ? NamedTexture::texStorage2DMultisample(samples, internal) : texImage2DMultiSample(target, samples, internal);
            break;
        case Type::CubeMap:
        case Type::Tex2D:
        case Type::Tex3D:
        case Type::Tex2DArray:
        case Type::TexCubeMapArray:
            glDeleteTextures(1, &id);
            throw std::logic_error("Wrong constructor usage to build texture.");
    }
}

NamedTexture::NamedTexture(Type target, GLsizei levels, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const void* data, const texture_parameters& params)
        : StateTexture(0, target, { size.x, size.y, 0 }, internal, format, data_type) {
    glCreateTextures(static_cast<GLenum>(target), 1, &id);

    params(*this);

    switch (target) {
        case Type::Tex2D:
            texStorage2D(levels, internal);
            if (data) {
                NamedTexture::texSubImage2D(0, 0, size, data);
            }
            break;
        case Type::Tex2DArray:
        case Type::Tex3D:
        case Type::CubeMap:
        case Type::TexCubeMapArray:
        case Type::Tex2DMS:
            glDeleteTextures(1, &id);
            throw std::logic_error("Wrong constructor usage to build texture.");
    }
}

NamedTexture::NamedTexture(StateTexture::Type target, GLsizei levels, StateTexture::InternalFormat internal, glm::uvec3 size, StateTexture::Format format, StateTexture::DataType data_type, const void *data, const texture_parameters& params)
        : StateTexture(0, target, size, internal, format, data_type) {
    glCreateTextures(static_cast<GLenum>(target), 1, &id);

    params(*this);

    switch (target) {
        case Type::Tex2DArray:
            [[fallthrough]];
        case Type::Tex3D:
            texStorage3D(levels, internal, size);
            if (data) {
                NamedTexture::texSubImage3D(0, 0, 0, size, data);
            }
            break;
        case Type::TexCubeMapArray:
            texStorage3D(levels, internal, { size.x, size.y, size.z * 6 });
            if (data) {
                NamedTexture::texSubImage3D(0, 0, 0, { size.x, size.y, size.z * 6 }, data);
            }
            break;
        case Type::Tex2D:
        case Type::CubeMap:
        case Type::Tex2DMS:
            glDeleteTextures(1, &id);
            throw std::logic_error("Wrong constructor usage to build texture.");
    }
}

NamedTexture::NamedTexture(StateTexture::Type target, GLsizei levels, StateTexture::InternalFormat internal, glm::uvec2 size, StateTexture::Format format, StateTexture::DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params)
        : StateTexture(0, target, { size.x, size.y, 0 }, internal, format, data_type) {
    glCreateTextures(static_cast<GLenum>(target), 1, &id);

    params(*this);

    switch (target) {
        case Type::CubeMap:
            StateTexture::texStorage2D(GL_TEXTURE_CUBE_MAP, levels, internal);
            for (int i = 0; i < 6; ++i) {
                if (data[i]) {
                    NamedTexture::texSubImage3D(0, 0, 0, { size.x, size.y, i }, data[i]);
                }
            }
            break;
        case Type::Tex2D:
        case Type::Tex3D:
        case Type::Tex2DArray:
        case Type::TexCubeMapArray:
        case Type::Tex2DMS:
            glDeleteTextures(1, &id);
            throw std::logic_error("Wrong constructor usage to build texture.");
    }
}

void NamedTexture::accept(TextureVisitor &visitor) const noexcept {
    visitor.visit(*this);
}