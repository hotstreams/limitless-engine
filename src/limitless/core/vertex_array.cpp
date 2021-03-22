#include <limitless/core/vertex_array.hpp>
#include <limitless/core/context_state.hpp>
#include <limitless/core/buffer.hpp>

using namespace LimitlessEngine;

VertexArray::VertexArray(GLuint id) noexcept : id(id), next_attribute_index(0) {}

VertexArray::VertexArray() noexcept : id(0), next_attribute_index(0) {
    glGenVertexArrays(1, &id);
}

VertexArray::~VertexArray() {
    if (id != 0) {
        if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
            if (state->vertex_array_id == id) {
                state->vertex_array_id = 0;
            }
            glDeleteVertexArrays(1, &id);
        }
    }
}

void VertexArray::bind() const noexcept {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        if (state->vertex_array_id != id) {
            glBindVertexArray(id);
            state->vertex_array_id = id;
        }
    }
}

void VertexArray::enableAttribute(GLuint index) const noexcept {
    glEnableVertexAttribArray(index);
}

void VertexArray::disableAttribute(GLuint index) const noexcept {
    glDisableVertexAttribArray(index);
}

VertexArray& VertexArray::operator<<(const VertexAttribute& attribute) noexcept {
    bind();

    const auto& [size, type, normalized, stride, pointer, buffer] = attribute;

    buffer.bind();

    enableAttribute(next_attribute_index);

    if (type == GL_INT) {
        glVertexAttribIPointer(next_attribute_index, size, type, stride, pointer);
    } else {
        glVertexAttribPointer(next_attribute_index, size, type, normalized, stride, pointer);
    }

    ++next_attribute_index;

    return *this;
}

VertexArray& VertexArray::operator<<(const std::pair<Vertex, Buffer&>& attribute) noexcept {
    *this << VertexAttribute{ 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position), attribute.second }
          << VertexAttribute{ 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv), attribute.second };

    return *this;
}

VertexArray& VertexArray::operator<<(const std::pair<VertexNormal, Buffer&>& attribute) noexcept {
    *this << VertexAttribute{ 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (GLvoid*)offsetof(Vertex, position), attribute.second }
          << VertexAttribute{ 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (GLvoid*)offsetof(VertexNormal, normal), attribute.second }
          << VertexAttribute{ 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (GLvoid*)offsetof(VertexNormal, uv), attribute.second };

    return *this;
}

VertexArray& VertexArray::operator<<(const std::pair<VertexNormalTangent, Buffer&>& attribute) noexcept {
    *this << VertexAttribute{ 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTangent), (GLvoid*)offsetof(Vertex, position), attribute.second }
          << VertexAttribute{ 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTangent), (GLvoid*)offsetof(VertexNormalTangent, normal), attribute.second }
          << VertexAttribute{ 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTangent), (GLvoid*)offsetof(VertexNormalTangent, tangent), attribute.second }
          << VertexAttribute{ 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTangent), (GLvoid*)offsetof(VertexNormalTangent, uv), attribute.second };

    return *this;
}

VertexArray& VertexArray::operator<<(const std::pair<VertexPackedNormalTangent, Buffer&>& attribute) noexcept {
    *this << VertexAttribute{ 3, GL_FLOAT, GL_FALSE, sizeof(VertexPackedNormalTangent), (GLvoid*)offsetof(Vertex, position), attribute.second }
          << VertexAttribute{ 4, GL_INT_2_10_10_10_REV, GL_TRUE, sizeof(VertexPackedNormalTangent), (GLvoid*)offsetof(VertexPackedNormalTangent, normal), attribute.second }
          << VertexAttribute{ 4, GL_INT_2_10_10_10_REV, GL_TRUE, sizeof(VertexPackedNormalTangent), (GLvoid*)offsetof(VertexPackedNormalTangent, tangent), attribute.second }
          << VertexAttribute{ 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(VertexPackedNormalTangent), (GLvoid*)offsetof(VertexPackedNormalTangent, uv), attribute.second };

    return *this;
}

VertexArray& VertexArray::operator<<(const Buffer& element_buffer) noexcept {
    bind();
    element_buffer.bind();
    return *this;
}

VertexArray::VertexArray(VertexArray&& rhs) noexcept : VertexArray(0) {
    swap(*this, rhs);
}

VertexArray &VertexArray::operator=(VertexArray&& rhs) noexcept {
    swap(*this, rhs);
    return *this;
}

void LimitlessEngine::swap(VertexArray& lhs, VertexArray& rhs) {
    using std::swap;

    swap(lhs.id, rhs.id);
    swap(lhs.next_attribute_index, rhs.next_attribute_index);
}

VertexArray& VertexArray::operator<<(const std::pair<TextVertex, Buffer&>& attribute) noexcept {
    *this << VertexAttribute{ 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (GLvoid*)offsetof(TextVertex, position), attribute.second }
          << VertexAttribute{ 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (GLvoid*)offsetof(TextVertex, uv), attribute.second };

    return *this;
}
