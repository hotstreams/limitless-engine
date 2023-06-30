#include <limitless/core/shader/shader_program.hpp>

#include <limitless/core/shader/shader_program_introspection.hpp>
#include <limitless/core/shader/shader_program_texture_setter.hpp>
#include <limitless/core/uniform/uniform_sampler.hpp>
#include <limitless/core/context_state.hpp>
#include <limitless/ms/material.hpp>
#include <algorithm>

using namespace Limitless;

ShaderProgram::ShaderProgram(GLuint id) noexcept
    : id {id}
    , locations {ShaderProgramIntrospection::getUniformLocations(id)}
    , indexed_binds {ShaderProgramIntrospection::getIndexedBufferBounds(id)} {
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

        if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
            // binds buffer to state binding point
            try {
                auto buffer = state->getIndexedBuffers().get(name);

                Buffer::Type program_target {};
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
                continue;
            }
        }
    }
}

void ShaderProgram::bindResources() {
    ShaderProgramTextureSetter::bindTextures(uniforms);
    bindIndexedBuffers();
}

ShaderProgram::~ShaderProgram() {
    if (id != 0) {
        if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
            if (state->shader_id == id) {
                state->shader_id = 0;
            }
        }
        glDeleteProgram(id);
    }
}

void ShaderProgram::use() {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        if (state->shader_id != id) {
            state->shader_id = id;
            glUseProgram(id);
        }

        bindResources();

        for (auto& [name, uniform] : uniforms) {
            uniform->set();
        }
    }
}

template<typename T>
ShaderProgram& ShaderProgram::setUniform(const std::string& name, const T& value) {
    // if not present, add new
    if (auto it = uniforms.find(name); it == uniforms.end()) {
        try {
            uniforms[name] = std::make_unique<UniformValue<T>>(name, locations.at(name), value);
        } catch (...) {
            throw std::runtime_error("Looks like this got optimized out");
        }
    } else {
        // else just update value
        static_cast<UniformValue<T>&>(*it->second).setValue(value);
    }

    return *this;
}

ShaderProgram& ShaderProgram::setUniform(const std::string& name, std::shared_ptr<Texture> texture) {
    // if not present, add new
    if (auto it = uniforms.find(name); it == uniforms.end()) {
        try {
            uniforms[name] = std::make_unique<UniformSampler>(name, locations.at(name), std::move(texture));
        } catch (...) {
            throw std::runtime_error("Looks like this got optimized out");
        }
    } else {
        // else just update value
        static_cast<UniformSampler&>(*it->second).setSampler(std::move(texture)); //NOLINT
    }

    return *this;
}

ShaderProgram& ShaderProgram::setMaterial(const ms::Material& material) {
    // if not present for whatever reason just return
    auto found = std::find_if(indexed_binds.begin(), indexed_binds.end(), [] (const auto& buf) { return buf.name == "material_buffer"; });
    if (found == indexed_binds.end()) {
        return *this;
    }

    // bind current material buffer to shader
    // these contain scalar values
    material.getMaterialBuffer()->bindBase(found->bound_point);

    // and we need explicitly set samplers
    for (const auto& [type, uniform] : material.getProperties()) {
        if (uniform->getType() == UniformType::Sampler) {
            setUniform(uniform->getName(), static_cast<UniformSampler&>(*uniform).getSampler()); //NOLINT
        }
    }

    for (const auto& [name, uniform] : material.getUniforms()) {
        if (uniform->getType() == UniformType::Sampler) {
            setUniform(uniform->getName(), static_cast<UniformSampler&>(*uniform).getSampler()); //NOLINT
        }
    }

    return *this;
}
