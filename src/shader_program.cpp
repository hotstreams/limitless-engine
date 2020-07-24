#include <shader_program.hpp>
#include <iostream>
#include <texture_binder.hpp>
#include <algorithm>
#include <bindless_texture.hpp>

using namespace GraphicsEngine;

ShaderProgram::ShaderProgram() : id(0) {}

ShaderProgram::ShaderProgram(GLuint id) : id(id) {
    getUniformLocations();
    getIndexedBufferBounds();
}

void ShaderProgram::use() {
    auto *window = glfwGetCurrentContext();
    if (ContextState::hasState(window)) {
        auto &current_id = ContextState::getState(window).shader_id;

        if (current_id != id) {
            glUseProgram(id);
            current_id = id;
        }

        bindIndexedBuffers();

        if (!ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
            // collects textures
            // binds them to units for current usage
            // sets unit index value to samplers in shader
            std::vector<const Texture *> to_bind;
            for (const auto&[name, uniform] : uniforms) {
                if (uniform->getType() == UniformType::Sampler) {
                    const auto &sampler = static_cast<UniformSampler &>(*uniform);
                    to_bind.emplace_back(sampler.getSampler().get());
                }
            }

            const auto units = TextureBinder::bind(to_bind);

            uint32_t i = 0;
            for (const auto&[name, uniform] : uniforms) {
                if (uniform->getType() == UniformType::Sampler) {
                    auto &sampler = static_cast<UniformSampler &>(*uniform);
                    sampler.setValue(units[i++]);
                }
            }
        }

        for (const auto&[name, uniform] : uniforms) {
            uniform->set(*this);
        }
    }
}

ShaderProgram::ShaderProgram(ShaderProgram&& rhs) noexcept : ShaderProgram() {
    swap(*this, rhs);
}

ShaderProgram&  ShaderProgram::operator=(ShaderProgram&& rhs) noexcept {
    swap(*this, rhs);
    return *this;
}

void GraphicsEngine::swap(ShaderProgram& lhs, ShaderProgram& rhs) noexcept {
    using std::swap;

    swap(lhs.id, rhs.id);
    swap(lhs.locations, rhs.locations);
    swap(lhs.indexed_binds, rhs.indexed_binds);
    swap(lhs.uniforms, rhs.uniforms);
}

void ShaderProgram::setUniform(const UniformValue<glm::vec2>& uniform) const {
    auto location = locations.find(uniform.getName());
    if (location == locations.end()) {
        return;
    }

    glUniform2fv(location->second, 1, &uniform.getValue()[0]);
}

void ShaderProgram::setUniform(const UniformValue<glm::vec3>& uniform) const {
    auto location = locations.find(uniform.getName());
    if (location == locations.end()) {
        return;
    }

    glUniform3fv(location->second, 1, &uniform.getValue()[0]);
}

void ShaderProgram::setUniform(const UniformValue<glm::vec4>& uniform) const {
    auto location = locations.find(uniform.getName());
    if (location == locations.end()) {
        return;
    }

    glUniform4fv(location->second, 1, &uniform.getValue()[0]);
}

void ShaderProgram::setUniform(const UniformValue<glm::mat3>& uniform) const {
    auto location = locations.find(uniform.getName());
    if (location == locations.end()) {
        return;
    }

    glUniformMatrix3fv(location->second, 1, GL_FALSE, &uniform.getValue()[0][0]);
}

void ShaderProgram::setUniform(const UniformValue<glm::mat4>& uniform) const {
    auto location = locations.find(uniform.getName());
    if (location == locations.end()) {
        return;
    }

    glUniformMatrix4fv(location->second, 1, GL_FALSE, &uniform.getValue()[0][0]);
}

void ShaderProgram::setUniform(const UniformValue<unsigned int>& uniform) const {
    auto location = locations.find(uniform.getName());
    if (location == locations.end()) {
        return;
    }

    glUniform1ui(location->second, uniform.getValue());
}

void ShaderProgram::setUniform(const UniformValue<float>& uniform) const {
    auto location = locations.find(uniform.getName());
    if (location == locations.end()) {
        return;
    }

    glUniform1f(location->second, uniform.getValue());
}

void ShaderProgram::setUniform(const UniformValue<int>& uniform) const {
    auto location = locations.find(uniform.getName());
    if (location == locations.end()) {
        return;
    }

    glUniform1i(location->second, uniform.getValue());
}

void ShaderProgram::setUniform(const UniformSampler& uniform) const {
    auto location = locations.find(uniform.getName());
    if (location == locations.end()) {
        return;
    }

    if (ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
        glUniformHandleui64ARB(location->second, static_cast<BindlessTexture&>(*uniform.getSampler()).getHandle());
    } else {
        setUniform(static_cast<const UniformValue<int>&>(uniform));
    }
}

void ShaderProgram::getUniformLocations() noexcept {
    GLint uniform_count = 0;
    glGetProgramInterfaceiv(id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &uniform_count);

    const std::array<GLenum, 3> props = { GL_BLOCK_INDEX, GL_NAME_LENGTH, GL_LOCATION };

    for (int i = 0; i < uniform_count; ++i) {
        std::array<GLint, 3> values = { 0 };
        glGetProgramResourceiv(id, GL_UNIFORM, i, 3, props.data(), 3, nullptr, values.data());

        if (values[0] != -1) continue;

        std::string name;
        name.resize(values[1] - 1);

        glGetProgramResourceName(id, GL_UNIFORM, i, values[1], nullptr, name.data());
        locations.emplace(name, values[2]);
    }
}

void ShaderProgram::getIndexedBufferBounds() noexcept {
    using IndxType = IndexedBuffer::Type;
    std::array<IndxType, 2> types = { IndxType::UniformBuffer, IndxType::ShaderStorage };

    for (const auto& type : types) {
        GLint count = 0;
        glGetProgramInterfaceiv(id, static_cast<GLenum>(type), GL_ACTIVE_RESOURCES, &count);

        GLenum props = { GL_NAME_LENGTH };

        for (int i = 0; i < count; ++i) {
            GLint value = 0;
            glGetProgramResourceiv(id, static_cast<GLenum>(type), i, 1, &props, 1, nullptr, &value);

            std::string name;
            name.resize(value - 1);

            glGetProgramResourceName(id, static_cast<GLenum>(type), i, value, nullptr, name.data());

            GLint index = glGetProgramResourceIndex(id, static_cast<GLenum>(type), name.data());

            indexed_binds.emplace_back(IndexedBufferData{type, name, index, IndexedBuffer::getBindingPoint({type, name }), false });
        }
    }
}

ShaderProgram::~ShaderProgram() {
    if (id != 0) {
        glDeleteProgram(id);
    }
}

void ShaderProgram::bindIndexedBuffers() {
    for (auto& [target, name, block_index, bound_point, connected] : indexed_binds) {
        // connects index block inside program with state binding point
        if (!connected) {
            switch (target) {
                case IndexedBuffer::Type::UniformBuffer:
                    glUniformBlockBinding(id, block_index, bound_point);
                    connected = true;
                    break;
                case IndexedBuffer::Type::ShaderStorage:
                    glShaderStorageBlockBinding(id, block_index, bound_point);
                    connected = true;
                    break;
            }
        }

        // binds buffer to state binding point
        try {
            auto buffer = IndexedBuffer::get(name);

            Buffer::Type program_target;
            switch (target) {
                case IndexedBuffer::Type::UniformBuffer:
                    program_target = Buffer::Type::Uniform;
                    break;
                case IndexedBuffer::Type::ShaderStorage:
                    program_target = Buffer::Type::ShaderStorage;
                    break;
            }

            buffer->bindBaseAs(program_target, bound_point);
        } catch (const buffer_not_found& e) {
            throw e;
        } catch (const multiple_indexed_buffers& e) {
            continue;
        }
    }
}

ShaderProgram& ShaderProgram::operator<<(const UniformSampler& uniform) noexcept {
    auto find = uniforms.find(uniform.getName());
    if (find != uniforms.end()) {
        static_cast<UniformSampler&>(*find->second).setSampler(uniform.getSampler());
    } else {
        uniforms.emplace(uniform.getName(), new UniformSampler(uniform));
    }
    return *this;
}

ShaderProgram& ShaderProgram::operator<<(const Material& material) {
    material.update();

    auto found = std::find_if(indexed_binds.begin(), indexed_binds.end(), [] (const auto& buf) { return buf.name == "material_buffer"; });
    if (found == indexed_binds.end()) {
        throw std::runtime_error("There is no material in shader.");
    }

    material.material_buffer->bindBase(found->bound_point);

    if (!ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
        for (const auto& [type, uniform] : material.properties) {
            if (uniform->getType() == UniformType::Sampler) {
                *this << static_cast<UniformSampler&>(*uniform);
            }
        }
    }

    return *this;
}

template<typename T>
ShaderProgram& ShaderProgram::operator<<(const UniformValue<T>& uniform) noexcept {
    auto found = uniforms.find(uniform.getName());
    if (found != uniforms.end()) {
        static_cast<UniformValue<T>&>(*found->second).setValue(uniform.getValue());
    } else {
        uniforms.emplace(uniform.getName(), new UniformValue(uniform));
    }
    return *this;
}

namespace GraphicsEngine {
    template ShaderProgram& ShaderProgram::operator<<(const UniformValue<int>& uniform) noexcept;
    template ShaderProgram& ShaderProgram::operator<<(const UniformValue<float>& uniform) noexcept;
    template ShaderProgram& ShaderProgram::operator<<(const UniformValue<unsigned int>& uniform) noexcept;
    template ShaderProgram& ShaderProgram::operator<<(const UniformValue<glm::vec2>& uniform) noexcept;
    template ShaderProgram& ShaderProgram::operator<<(const UniformValue<glm::vec3>& uniform) noexcept;
    template ShaderProgram& ShaderProgram::operator<<(const UniformValue<glm::vec4>& uniform) noexcept;
    template ShaderProgram& ShaderProgram::operator<<(const UniformValue<glm::mat4>& uniform) noexcept;
}
