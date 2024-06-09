#include <limitless/core/vertex_array.hpp>
#include <limitless/core/context.hpp>
#include <limitless/core/buffer/buffer.hpp>
#include <utility>

using namespace Limitless;

VertexAttribute::VertexAttribute(GLint _size, GLenum _type, GLboolean _normalized, GLsizei _stride, const GLvoid* _pointer, std::shared_ptr<Buffer> buffer) noexcept
    : size {_size}
    , type {_type}
    , normalized {_normalized}
    , stride {_stride}
    , pointer {_pointer}
    , buffer {std::move(buffer)} {}

VertexArray::VertexArray() noexcept {
    glGenVertexArrays(1, &id);
}

VertexArray::~VertexArray() {
    if (id != 0) {
        Context::apply([this] (Context& ctx) {
            if (ctx.vertex_array_id == id) {
                ctx.vertex_array_id = 0;
            }
            glDeleteVertexArrays(1, &id);
        });
    }
}

void VertexArray::bind() const noexcept {
    Context::apply([this] (Context& ctx) {
        if (ctx.vertex_array_id != id) {
            glBindVertexArray(id);
            ctx.vertex_array_id = id;
        }
    });
}

void VertexArray::enable(GLuint index) const noexcept {
    bind();
    glEnableVertexAttribArray(index);
}

void VertexArray::disable(GLuint index) const noexcept {
    bind();
    glDisableVertexAttribArray(index);
}

VertexArray& VertexArray::operator<<(const std::pair<VertexNormalTangent, const std::shared_ptr<Buffer>&>& attribute) noexcept {
    setAttribute<glm::vec3>(0, false, sizeof(VertexNormalTangent), (GLvoid*)offsetof(VertexNormalTangent, position), attribute.second);
    setAttribute<glm::vec3>(1, false, sizeof(VertexNormalTangent), (GLvoid*)offsetof(VertexNormalTangent, normal), attribute.second);
    setAttribute<glm::vec3>(2, false, sizeof(VertexNormalTangent), (GLvoid*)offsetof(VertexNormalTangent, tangent), attribute.second);
    setAttribute<glm::vec2>(3, false, sizeof(VertexNormalTangent), (GLvoid*)offsetof(VertexNormalTangent, uv), attribute.second);
    return *this;
}

VertexArray& VertexArray::operator<<(const std::pair<VertexTerrain, const std::shared_ptr<Buffer>&>& attribute) noexcept {
    setAttribute<glm::vec3>(0, false, sizeof(VertexTerrain), (GLvoid*)offsetof(VertexTerrain, position), attribute.second);
    setAttribute<glm::vec3>(1, false, sizeof(VertexTerrain), (GLvoid*)offsetof(VertexTerrain, normal), attribute.second);
    setAttribute<glm::vec3>(2, false, sizeof(VertexTerrain), (GLvoid*)offsetof(VertexTerrain, tangent), attribute.second);
    setAttribute<glm::vec2>(3, false, sizeof(VertexTerrain), (GLvoid*)offsetof(VertexTerrain, uv), attribute.second);
    setAttribute<glm::vec2>(4, false, sizeof(VertexTerrain), (GLvoid*)offsetof(VertexTerrain, uv1), attribute.second);
    setAttribute<glm::vec2>(5, false, sizeof(VertexTerrain), (GLvoid*)offsetof(VertexTerrain, uv2), attribute.second);
    setAttribute<glm::vec2>(6, false, sizeof(VertexTerrain), (GLvoid*)offsetof(VertexTerrain, uv3), attribute.second);
    setAttribute<uint32_t>(7, false, sizeof(VertexTerrain), (GLvoid*)offsetof(VertexTerrain, current), attribute.second);
    setAttribute<uint32_t>(8, false, sizeof(VertexTerrain), (GLvoid*)offsetof(VertexTerrain, mask), attribute.second);
    setAttribute<uint32_t>(9, false, sizeof(VertexTerrain), (GLvoid*)offsetof(VertexTerrain, types), attribute.second);
    return *this;
}

VertexArray::VertexArray(VertexArray&& rhs) noexcept {
    swap(*this, rhs);
}

VertexArray& VertexArray::operator=(VertexArray&& rhs) noexcept {
    swap(*this, rhs);
    return *this;
}

void Limitless::swap(VertexArray& lhs, VertexArray& rhs) {
    using std::swap;

    swap(lhs.id, rhs.id);
}

VertexArray& VertexArray::operator<<(const std::pair<Vertex, const std::shared_ptr<Buffer>&>& attribute) noexcept {
    setAttribute<glm::vec2>(0, false, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position), attribute.second);
    setAttribute<glm::vec2>(1, false, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv), attribute.second);
    return *this;
}

VertexArray& VertexArray::operator<<(const std::pair<TextVertex, const std::shared_ptr<Buffer>&>& attribute) noexcept {
    setAttribute<glm::vec2>(0, false, sizeof(TextVertex), (GLvoid*)offsetof(TextVertex, position), attribute.second);
    setAttribute<glm::vec2>(1, false, sizeof(TextVertex), (GLvoid*)offsetof(TextVertex, uv), attribute.second);
	return *this;
}

void VertexArray::setAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer, const std::shared_ptr<Buffer>& buffer) {
    bind();

    buffer->bind();

    enable(index);

    if (type == GL_INT || type == GL_UNSIGNED_INT) {
        glVertexAttribIPointer(index, size, type, stride, pointer);
    } else {
        glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    }

    attributes.insert_or_assign(index, VertexAttribute{size, type, normalized, stride, pointer, buffer});
}

void VertexArray::setElementBuffer(const std::shared_ptr<Buffer>& buffer) {
    bind();
    buffer->bind();

    if (!element_buffer || element_buffer->getId() != buffer->getId()) {
        element_buffer = buffer;
    }
}

VertexArray::VertexArray(const VertexArray& rhs)
    : attributes {rhs.attributes}
    , element_buffer {rhs.element_buffer} {
    glGenVertexArrays(1, &id);

    for (const auto& [index, attribute] : attributes) {
        const auto& [size, type, normalized, stride, pointer, buffer] = attribute;
        setAttribute(index, size, type, normalized, stride, pointer, buffer);
    }

    if (element_buffer) {
        setElementBuffer(element_buffer);
    }
}
