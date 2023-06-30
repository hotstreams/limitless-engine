#include <limitless/core/shader/shader_program_introspection.hpp>

#include <limitless/core/context.hpp>

using namespace Limitless;

std::unordered_map<std::string, GLint> ShaderProgramIntrospection::uniformLocations_withProgramInterfaceQuery(GLuint id) {
    std::unordered_map<std::string, GLint> locations;

    GLint uniform_count = 0;

    glGetProgramInterfaceiv(id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &uniform_count);

    const std::array<GLenum, 3> props = { GL_BLOCK_INDEX, GL_NAME_LENGTH, GL_LOCATION };

    for (GLint i = 0; i < uniform_count; ++i) {
        std::array<GLint, 3> values = { 0 };
        glGetProgramResourceiv(id, GL_UNIFORM, i, props.size(), props.data(), values.size(), nullptr, values.data());

        if (values[0] != -1) continue;

        std::string name;
        name.resize(static_cast<uint32_t>(values[1]) - 1UL);

        glGetProgramResourceName(id, GL_UNIFORM, i, values[1], nullptr, name.data());
        locations.emplace(name, values[2]);
    }

    return locations;
}

std::vector<IndexedBufferData> ShaderProgramIntrospection::getIndexedBufferBounds(GLuint id) {
    std::vector<IndexedBufferData> indexed_binds;

    std::array types = { IndexedBuffer::Type::UniformBuffer, IndexedBuffer::Type::ShaderStorage };

    for (const auto& type : types) {
        GLint count = 0;
        glGetProgramInterfaceiv(id, static_cast<GLenum>(type), GL_ACTIVE_RESOURCES, &count);

        const GLenum props = { GL_NAME_LENGTH };

        for (GLint i = 0; i < count; ++i) {
            GLint value = 0;
            glGetProgramResourceiv(id, static_cast<GLenum>(type), i, 1, &props, 1, nullptr, &value);

            std::string name;
            name.resize(value - 1);

            glGetProgramResourceName(id, static_cast<GLenum>(type), i, value, nullptr, name.data());

            const auto index = glGetProgramResourceIndex(id, static_cast<GLenum>(type), name.data());

            if (auto *ctx = ContextState::getState(glfwGetCurrentContext()); ctx) {
                indexed_binds.emplace_back(type, name, index, ctx->getIndexedBuffers().getBindingPoint(type, name));
            }
        }
    }

    return indexed_binds;
}
