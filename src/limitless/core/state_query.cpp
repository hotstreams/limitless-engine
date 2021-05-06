#include <limitless/core/state_query.hpp>

using namespace Limitless;

GLint StateQuery::geti(QueryState name) {
    GLint value {};
    glGetIntegerv(static_cast<GLenum>(name), &value);
    return value;
}

GLfloat StateQuery::getf(QueryState name) {
    GLfloat value {};
    glGetFloatv(static_cast<GLenum>(name), &value);
    return value;
}

GLboolean StateQuery::getb(QueryState name) {
    GLboolean value {};
    glGetBooleanv(static_cast<GLenum>(name), &value);
    return value;
}

glm::vec4 StateQuery::get4f(QueryState name) {
    GLfloat value[4] {};
    glGetFloatv(static_cast<GLenum>(name), value);
    return { value[0], value[1], value[2], value[3] };
}

glm::vec3 StateQuery::get3f(QueryState name) {
    GLfloat value[3] {};
    glGetFloatv(static_cast<GLenum>(name), value);
    return { value[0], value[1], value[2] };
}

glm::uvec4 StateQuery::get4ui(QueryState name) {
    GLint value[4] {};
    glGetIntegerv(static_cast<GLenum>(name), value);
    return { value[0], value[1], value[2], value[3] };
}

GLint StateQuery::getTexi(GLenum target, GLenum pname) {
    GLint value {};
    glGetTexParameteriv(target, pname, &value);
    return value;
}


//void MaterialBuilder::initializeMaterialBuffer(Material& material, const ShaderProgram& shader) {
//    if (material.material_buffer) {
//        return;
//    }
//
//    const auto found = std::find_if(shader.indexed_binds.begin(), shader.indexed_binds.end(), [&] (const auto& data) { return data.name == MATERIAL_SHADER_BUFFER_NAME; });
//    const auto block_index = found->block_index;
//
//    const std::array<GLenum, 2> block_properties = { GL_NUM_ACTIVE_VARIABLES, GL_BUFFER_DATA_SIZE };
//    const GLenum active_variables = GL_ACTIVE_VARIABLES;
//    const std::array<GLenum, 2> uniform_prop = { GL_NAME_LENGTH, GL_OFFSET };
//
//    std::array<GLint, 2> block_values = { 0 };
//    glGetProgramResourceiv(shader.getId(), GL_UNIFORM_BLOCK, block_index, block_properties.size(), block_properties.data(), block_values.size(), nullptr, block_values.data());
//
//    BufferBuilder builder;
//    material.material_buffer = builder  .setTarget(Buffer::Type::Uniform)
//                                        .setUsage(Buffer::Usage::DynamicDraw)
//                                        .setAccess(Buffer::MutableAccess::WriteOrphaning)
//                                        .setDataSize(sizeof(std::byte) * block_values[1])
//                                        .build();
//
//    std::vector<GLint> block_uniforms(block_values[0]);
//    glGetProgramResourceiv(shader.getId(), GL_UNIFORM_BLOCK, block_index, 1, &active_variables, block_values[0], nullptr, block_uniforms.data());
//
//    // querying variable offsets in uniform buffer used for material properties to map it with byte presentation
//    for (GLint i = 0; i < block_values[0]; ++i) {
//        std::array<GLint, 2> values = { 0 };
//        glGetProgramResourceiv(shader.getId(), GL_UNIFORM, block_uniforms[i], uniform_prop.size(), uniform_prop.data(), values.size(), nullptr, values.data());
//
//        std::string name;
//        name.resize(values[0] - 1);
//
//        glGetProgramResourceName(shader.getId(), GL_UNIFORM, block_uniforms[i], values[0], nullptr, name.data());
//
//        material.uniform_offsets.emplace(name, values[1]);
//    }
//}