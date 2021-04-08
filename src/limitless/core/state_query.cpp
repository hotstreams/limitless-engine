#include <limitless/core/state_query.hpp>

using namespace LimitlessEngine;

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
