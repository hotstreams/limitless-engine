#include <core/state_texture.hpp>
#include <core/context_state.hpp>
#include <stdexcept>
#include <iostream>

using namespace GraphicsEngine;

StateTexture::StateTexture() noexcept
        : id(0), target(Type::Tex2D), size(0), internal(InternalFormat::RGB), format(Format::RGB), data_type(DataType::Float) { }

StateTexture::StateTexture(GLuint id, Type target, glm::uvec3 size, InternalFormat internal, Format format, DataType data_type) noexcept
        : id(id), target(target), size(size), internal(internal), format(format), data_type(data_type) {}

StateTexture::StateTexture(Type target, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const void* data, const texture_parameters& params)
        : id(0), target(target), size({ size.x, size.y, 0 }), internal(internal), format(format), data_type(data_type) {
    glGenTextures(1, &id);

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

StateTexture::StateTexture(Type target, InternalFormat internal, glm::uvec3 size, Format format, DataType data_type, const void* data, const texture_parameters& params)
        : id(0), target(target), size(size), internal(internal), format(format), data_type(data_type) {
    glGenTextures(1, &id);

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

StateTexture::StateTexture(Type target, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params)
        : id(0), target(target), size({ size.x, size.y, 0 }), internal(internal), format(format), data_type(data_type) {
    glGenTextures(1, &id);

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

StateTexture::StateTexture(Type target, uint8_t samples, InternalFormat internal, glm::uvec2 size, bool immutable, const texture_parameters& params)
        : id(0), target(target), size({ size.x, size.y, 0 }), internal(internal), format{Format::RGB}, data_type{DataType::Byte}, samples(samples) {
    glGenTextures(1, &id);

    params(*this);

    switch (target) {
        case Type::Tex2DMS:
            immutable ? StateTexture::texStorage2DMultisample(samples, internal) : texImage2DMultiSample(target, samples, internal);
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

StateTexture::StateTexture(Type target, GLsizei levels, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const void* data, const texture_parameters& params)
        : id(0), target(target), size({ size.x, size.y, 0 }), internal(internal), format(format), data_type(data_type), immutable{true} {
    glGenTextures(1, &id);

    params(*this);

    switch (target) {
        case Type::Tex2D:
            texStorage2D(static_cast<GLenum>(target), levels, internal);
            if (data) {
                StateTexture::texSubImage2D(0, 0, size, data);
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

StateTexture::StateTexture(StateTexture::Type target, GLsizei levels, StateTexture::InternalFormat internal, glm::uvec3 size, StateTexture::Format format, StateTexture::DataType data_type, const void *data, const texture_parameters& params)
        : id(0), target(target), size(size), internal(internal), format(format), data_type(data_type), immutable{true} {
    glGenTextures(1, &id);

    params(*this);

    switch (target) {
        case Type::Tex2DArray:
            [[fallthrough]];
        case Type::Tex3D:
            texStorage3D(static_cast<GLenum>(target), levels, internal, size);
            if (data) {
                StateTexture::texSubImage3D(0, 0, 0, size, data);
            }
            break;
        case Type::TexCubeMapArray:
            texStorage3D(static_cast<GLenum>(target), levels, internal, { size.x, size.y, size.z * 6 });
            if (data) {
                StateTexture::texSubImage3D(0, 0, 0, { size.x, size.y, size.z * 6 }, data);
            }
            break;
        case Type::Tex2D:
        case Type::CubeMap:
        case Type::Tex2DMS:
            glDeleteTextures(1, &id);
            throw std::logic_error("Wrong constructor usage to build texture.");
    }
}

StateTexture::StateTexture(StateTexture::Type target, GLsizei levels, StateTexture::InternalFormat internal, glm::uvec2 size, StateTexture::Format format, StateTexture::DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params)
        : id(0), target(target), size({ size.x, size.y, 0 }), internal(internal), format(format), data_type(data_type), immutable{true} {
    glGenTextures(1, &id);

    params(*this);

    switch (target) {
        case Type::CubeMap:
            texStorage2D(GL_TEXTURE_CUBE_MAP, levels, internal);
            for (int i = 0; i < 6; ++i) {
                if (data[i]) {
                    StateTexture::texSubImage2D(0, 0, size, data[i]);
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

void StateTexture::texImage2D(GLenum type, InternalFormat _internal, const void* data) const noexcept {
    StateTexture::bind(0);

    glTexImage2D(type, 0, static_cast<GLint>(_internal), size.x, size.y, 0, static_cast<GLenum>(format), static_cast<GLenum>(data_type), data);
}

void StateTexture::texImage3D(GLenum type, InternalFormat _internal, glm::uvec3 _size, const void* data) const noexcept {
    StateTexture::bind(0);

    glTexImage3D(static_cast<GLenum>(type), 0, static_cast<GLint>(_internal), _size.x, _size.y, _size.z, 0, static_cast<GLenum>(format), static_cast<GLenum>(data_type), data);
}

void StateTexture::texImage2DMultiSample(Type type, uint8_t _samples, InternalFormat _internal) const noexcept {
    StateTexture::bind(0);

    glTexImage2DMultisample(static_cast<GLenum>(type), _samples, static_cast<GLint>(_internal), size.x, size.y, GL_FALSE);
}

void StateTexture::texStorage2D(GLenum type, GLsizei levels, InternalFormat _internal) const noexcept {
    StateTexture::bind(0);

    glTexStorage2D(type, levels, static_cast<GLenum>(_internal), size.x, size.y);
}

void StateTexture::texStorage3D(GLenum type, GLsizei levels, InternalFormat _internal, glm::uvec3 _size) const noexcept {
    StateTexture::bind(0);

    glTexStorage3D(type, levels, static_cast<GLenum>(_internal), _size.x, _size.y, _size.z);
}

void StateTexture::texStorage2DMultisample(uint8_t _samples, InternalFormat _internal) const noexcept {
    StateTexture::bind(0);

    glTexStorage2DMultisample(static_cast<GLenum>(target), _samples, static_cast<GLenum>(_internal), size.x, size.y, GL_FALSE);
}

StateTexture::~StateTexture() {
    if (id != 0) {
        auto* window = glfwGetCurrentContext();
        if (ContextState::hasState(window)) {
            auto& target_map = ContextState::getState(window)->texture_bound;
            for (auto& [unit, bound_id] : target_map) {
                if (bound_id == id) {
                    bound_id = 0;
                }
            }
            glDeleteTextures(1, &id);
        }
    }
}

void StateTexture::texSubImage2D(GLint xoffset, GLint yoffset, glm::uvec2 _size, const void* data) const noexcept {
    bind(0);

    glTexSubImage2D(static_cast<GLenum>(target), 0, xoffset, yoffset, _size.x, _size.y, static_cast<GLenum>(format), static_cast<GLenum>(data_type), data);
}

void StateTexture::texSubImage3D(GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 _size, const void* data) const noexcept {
    bind(0);

    glTexSubImage3D(static_cast<GLenum>(target), 0, xoffset, yoffset, zoffset, _size.x, _size.y, _size.z, static_cast<GLenum>(format), static_cast<GLenum>(data_type), data);
}

void StateTexture::activate(GLuint index) const noexcept {
    auto* window = glfwGetCurrentContext();
    if (ContextState::hasState(window)) {
        auto &activated = ContextState::getState(window)->active_texture;
        if (activated != index) {
            glActiveTexture(GL_TEXTURE0 + index);
            activated = index;
        }
    }
}

void StateTexture::bind(GLuint index) const noexcept {
    auto* window = glfwGetCurrentContext();
    if (ContextState::hasState(window)) {
        auto &bound = ContextState::getState(window)->texture_bound[index];
        if (bound != id) {
            activate(index);
            glBindTexture(static_cast<GLenum>(target), id);
            bound = id;
        }
    }
}

void StateTexture::resize(glm::uvec3 _size) {
    if (immutable) {
        throw std::runtime_error("Failed to resize immutable storage.");
    }

    if (size != _size) {
        size = _size;

        bind(0);

        switch (target) {
            case Type::Tex2D:
                texImage2D(static_cast<GLenum>(target), internal, nullptr);
                break;
            case Type::Tex2DArray:
            case Type::Tex3D:
                texImage3D(static_cast<GLenum>(target), internal, size, nullptr);
                break;
            case Type::CubeMap:
                for (int i = 0; i < 6; ++i) {
                    texImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, internal, nullptr);
                }
                break;
            case Type::TexCubeMapArray:
                texImage3D(static_cast<GLenum>(target), internal, { size.x, size.y, size.z * 6 }, nullptr);
                break;
            case Type::Tex2DMS:
                texImage2DMultiSample(target, samples, internal);
                break;
        }
    }
}

void StateTexture::generateMipMap() const noexcept {
    bind(0);

    glGenerateMipmap(static_cast<GLenum>(target));
}

StateTexture& StateTexture::operator<<(const TexParameter<GLint>& param) noexcept {
    bind(0);
    glTexParameteri(static_cast<GLenum>(target), param.name, param.param);
    return *this;
}

StateTexture& StateTexture::operator<<(const TexParameter<GLfloat>& param) noexcept {
    bind(0);
    glTexParameterf(static_cast<GLenum>(target), param.name, param.param);
    return *this;
}

StateTexture& StateTexture::operator<<(const TexParameter<GLint*>& param) noexcept {
    bind(0);
    glTexParameteriv(static_cast<GLenum>(target), param.name, param.param);
    return *this;
}

StateTexture& StateTexture::operator<<(const TexParameter<GLfloat*>& param) noexcept {
    bind(0);
    glTexParameterfv(static_cast<GLenum>(target), param.name, param.param);
    return *this;
}

GLuint StateTexture::getId() const noexcept {
    return id;
}

[[nodiscard]] Texture::Type StateTexture::getType() const noexcept {
    return target;
}

glm::uvec3 StateTexture::getSize() const noexcept {
    return size;
}

void StateTexture::accept(TextureVisitor &visitor) const noexcept {
    visitor.visit(*this);
}