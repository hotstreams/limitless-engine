#include <core/shader_program.hpp>

#include <algorithm>

#include <core/context_state.hpp>
#include <core/context_initializer.hpp>

#include <core/texture_binder.hpp>
#include <core/uniform.hpp>
#include <material_system/material.hpp>
#include <core/bindless_texture.hpp>
#include <iostream>

using namespace GraphicsEngine;

ShaderProgram::ShaderProgram(GLuint id) : id{id} {
    getUniformLocations();
    getIndexedBufferBounds();
}

GLint ShaderProgram::getUniformLocation(const Uniform& uniform) const noexcept {
    const auto location = locations.find(uniform.getName());
    if (location == locations.end()) {
        return -1;
    }

    return location->second;
}

void ShaderProgram::use() {
    if (auto* window = glfwGetCurrentContext(); ContextState::hasState(window)) {
        if (auto& current_id = ContextState::getState(window)->shader_id; current_id != id) {
            glUseProgram(id);
            current_id = id;
        }

        bindIndexedBuffers();
        bindTextures();

        for (auto& [name, uniform] : uniforms) {
            if (auto& changed = uniform->getChanged(); changed) {
                uniform->set(*this);
                changed = false;
            }
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

void ShaderProgram::getUniformLocations() noexcept {
    GLint uniform_count = 0;
    glGetProgramInterfaceiv(id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &uniform_count);

    const std::array<GLenum, 3> props = { GL_BLOCK_INDEX, GL_NAME_LENGTH, GL_LOCATION };

    for (int i = 0; i < uniform_count; ++i) {
        std::array<GLint, 3> values = { 0 };
        glGetProgramResourceiv(id, GL_UNIFORM, i, props.size(), props.data(), values.size(), nullptr, values.data());

        if (values[0] != -1) continue;

        std::string name;
        name.resize(static_cast<uint32_t>(values[1]) - 1UL);

        glGetProgramResourceName(id, GL_UNIFORM, i, values[1], nullptr, name.data());
        locations.emplace(name, values[2]);
    }
}

void ShaderProgram::getIndexedBufferBounds() noexcept {
    std::array types = { IndexedBuffer::Type::UniformBuffer, IndexedBuffer::Type::ShaderStorage };

    for (const auto& type : types) {
        GLint count = 0;
        glGetProgramInterfaceiv(id, static_cast<GLenum>(type), GL_ACTIVE_RESOURCES, &count);

        const GLenum props = { GL_NAME_LENGTH };

        for (int i = 0; i < count; ++i) {
            GLint value = 0;
            glGetProgramResourceiv(id, static_cast<GLenum>(type), i, 1, &props, 1, nullptr, &value);

            std::string name;
            name.resize(value - 1);

            glGetProgramResourceName(id, static_cast<GLenum>(type), i, value, nullptr, name.data());

            const auto index = glGetProgramResourceIndex(id, static_cast<GLenum>(type), name.data());

            indexed_binds.emplace_back(type, name, index, IndexedBuffer::getBindingPoint(type, name));
        }
    }
}

ShaderProgram::~ShaderProgram() {
    if (id != 0) {
        //TODO: del from state
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
                    break;
                case IndexedBuffer::Type::ShaderStorage:
                    glShaderStorageBlockBinding(id, block_index, bound_point);
                    break;
            }
            connected = true;
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
    //TODO: update before draw?
    material.update();

    auto found = std::find_if(indexed_binds.begin(), indexed_binds.end(), [] (const auto& buf) { return buf.name == "material_buffer"; });
    if (found == indexed_binds.end()) {
        throw std::runtime_error("There is no material in shader.");
    }

    material.material_buffer->bindBase(found->bound_point);

    for (const auto& [type, uniform] : material.properties) {
        if (uniform->getType() == UniformType::Sampler) {
            *this << static_cast<UniformSampler&>(*uniform);
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

struct TextureResidentMaker : public TextureVisitor {
    void visit(BindlessTexture& texture) noexcept override {
        texture.makeResident();
    }

    void visit([[maybe_unused]] Texture& texture) noexcept override {}
};

void ShaderProgram::bindTextures() const noexcept {
    //TODO: bind textures dependent on runtime type
    if (!ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
        // collects textures
        // binds them to units for current usage
        // sets unit index value to samplers in shader
        std::vector<Texture*> to_bind;
        for (const auto&[name, uniform] : uniforms) {
            if (uniform->getType() == UniformType::Sampler) {
                const auto &sampler = static_cast<UniformSampler&>(*uniform);
                to_bind.emplace_back(sampler.getSampler().get());
            }
        }

        const auto units = TextureBinder::bind(to_bind);

        uint32_t i = 0;
        for (const auto& [name, uniform] : uniforms) {
            if (uniform->getType() == UniformType::Sampler) {
                auto &sampler = static_cast<UniformSampler&>(*uniform);
                sampler.setValue(units[i++]);
            }
        }
    }

    // checks textures to be resident in bindless case
    TextureResidentMaker resident_maker;
    for (const auto&[name, uniform] : uniforms) {
        if (uniform->getType() == UniformType::Sampler) {
            auto &sampler = static_cast<UniformSampler&>(*uniform);
            sampler.getSampler()->accept(resident_maker);
        }
    }
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
