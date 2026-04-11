#include <limitless/core/shader/shader_program.hpp>

#include <limitless/core/shader/shader_program_introspection.hpp>
#include <limitless/core/shader/shader_program_texture_setter.hpp>
#include <limitless/core/uniform/uniform_sampler.hpp>
#include <limitless/core/uniform/uniform_value_array.hpp>
#include <limitless/core/context.hpp>
#include <limitless/ms/material.hpp>
#include <algorithm>
#include <limitless/core/cpu_profiler.hpp>

using namespace Limitless;

ShaderProgram::ShaderProgram(GLuint id) noexcept
    : id {id}
    , locations {ShaderProgramIntrospection::getUniformLocations(id)}
    , indexed_binds {ShaderProgramIntrospection::getIndexedBufferBounds(id)} {
}

void ShaderProgram::bindIndexedBuffers() {
    CpuProfileScope scope(global_profiler, "ShaderProgram::bindIndexedBuffers");
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

        if (auto* ctx = Context::getCurrentContext(); ctx) {
            // binds buffer to state binding point
            CpuProfileScope scope(global_profiler, "ShaderProgram::bindIndexedBuffers::bindBuffer");
            auto maybe_buffer = ctx->getIndexedBuffers().get(target, name);
            if (!maybe_buffer) {
                // usually this means that buffer is set manually, e.g. instanced instance buffer.
                CpuProfileScope scope(global_profiler, "ShaderProgram::bindIndexedBuffers::bindBuffer::bufferNotFound");
                continue;
            }

            auto& buffer = *maybe_buffer;

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
        }
    }
}

void ShaderProgram::bindResources() {
    ShaderProgramTextureSetter::bindTextures(uniforms);
    bindIndexedBuffers();
}

ShaderProgram::~ShaderProgram() {
    if (id != 0) {
        if (auto* state = Context::getCurrentContext(); state) {
            if (state->shader_id == id) {
                state->shader_id = 0;
            }
        }
        glDeleteProgram(id);
    }
}

void ShaderProgram::use() {
    if (auto* state = Context::getCurrentContext(); state) {
        if (state->shader_id != id) {
            CpuProfileScope scope(global_profiler, "ShaderProgram::use::useProgram");
            state->shader_id = id;
            glUseProgram(id);
        }

        {
            CpuProfileScope scope(global_profiler, "ShaderProgram::use::bindResources");
            bindResources();
        }

        {
            CpuProfileScope scope(global_profiler, "ShaderProgram::use::setUniforms");
            for (auto& [name, uniform] : uniforms) {
                uniform->set();
            }
        }
    }
}

template<typename T>
ShaderProgram& ShaderProgram::setUniform(const std::string& name, const T& value) {
    // if uniform got optimized out
    if (locations.find(name) == locations.end()) {
        return *this;
    }

    // if not present, add new
    if (auto it = uniforms.find(name); it == uniforms.end()) {
        uniforms[name] = std::make_unique<UniformValue<T>>(name, value);
        uniforms[name]->setLocation(locations.at(name));
    } else {
        // else just update value
        static_cast<UniformValue<T>&>(*it->second).setValue(value);
    }

    return *this;
}

template<typename T>
ShaderProgram& ShaderProgram::setUniform(const std::string& name, const std::vector<T>& values) {
    // uniform arrays are typically exposed as "name[0]" by GL introspection.
    // Accept both "name" and "name[0]" to be robust across drivers.
    std::string locName = name;
    auto locIt = locations.find(locName);
    if (locIt == locations.end()) {
        if (name.find('[') == std::string::npos) {
            locName = name + "[0]";
            locIt = locations.find(locName);
        }
        if (locIt == locations.end()) {
            // optimized out / not present
            return *this;
        }
    }

    // if not present, add new
    if (auto it = uniforms.find(locName); it == uniforms.end()) {
        uniforms[locName] = std::make_unique<UniformValueArray<T>>(locName, values);
        uniforms[locName]->setLocation(locIt->second);
    } else {
        // else just update value
        static_cast<UniformValueArray<T>&>(*it->second).getValues() = values; // marks changed
    }

    return *this;
}

ShaderProgram& ShaderProgram::setUniform(const std::string& name, std::shared_ptr<Texture> texture) {
    // if uniform got optimized out
    if (locations.find(name) == locations.end()) {
        return *this;
    }

    // if not present, add new
    if (auto it = uniforms.find(name); it == uniforms.end()) {
        uniforms[name] = std::make_unique<UniformSampler>(name, std::move(texture));
        uniforms[name]->setLocation(locations.at(name));
    } else {
        // else just update value
        static_cast<UniformSampler&>(*it->second).setSampler(std::move(texture)); //NOLINT
    }

    return *this;
}

ShaderProgram& ShaderProgram::setMaterial(const ms::Material& material) {
    // if not present for whatever reason just return
    auto found = std::find_if(indexed_binds.begin(), indexed_binds.end(), [] (const auto& buf) { return buf.name == "MATERIAL_BUFFER"; });
    if (found == indexed_binds.end()) {
        return *this;
    }

    // bind current material buffer to shader
    // these contain scalar values
    material.getBuffer().getBuffer()->bindBase(found->bound_point);

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

namespace Limitless {
    template ShaderProgram& ShaderProgram::setUniform(const std::string &name, const int32_t& value);
    template ShaderProgram& ShaderProgram::setUniform(const std::string &name, const uint32_t& value);
    template ShaderProgram& ShaderProgram::setUniform(const std::string &name, const float& value);
    template ShaderProgram& ShaderProgram::setUniform(const std::string &name, const glm::vec2& value);
    template ShaderProgram& ShaderProgram::setUniform(const std::string &name, const glm::vec3& value);
    template ShaderProgram& ShaderProgram::setUniform(const std::string &name, const glm::vec4& value);
    template ShaderProgram& ShaderProgram::setUniform(const std::string &name, const glm::mat3& value);
    template ShaderProgram& ShaderProgram::setUniform(const std::string &name, const glm::mat4& value);

    template ShaderProgram& ShaderProgram::setUniform(const std::string &name, const std::vector<glm::vec4>& values);
}
