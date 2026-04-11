#include <set>
#include <limitless/core/vertex_stream/vertex_array.hpp>
#include <limitless/core/vertex_stream/vertex_array_builder.hpp>
#include <limitless/core/context.hpp>
#include <limitless/core/buffer/buffer.hpp>
#include <utility>

#include <iostream>

using namespace Limitless;

VertexArray::Attribute::Attribute(GLint _size, GLenum _type, GLboolean _normalized, GLsizei _stride, const GLvoid* _pointer, std::shared_ptr<Buffer> buffer) noexcept
    : size {_size}
    , type {_type}
    , normalized {_normalized}
    , stride {_stride}
    , pointer {_pointer}
    , buffer {std::move(buffer)} {
}

void VertexArray::initialize() {
    for (const auto& [index, attribute] : attributes) {
        const auto& [size, type, normalized, stride, pointer, buffer] = attribute;
        setAttribute(index, size, type, normalized, stride, pointer, buffer);
    }

    if (element_buffer) {
        setElementBuffer(element_buffer);
    }

    Context::apply([] (Context& ctx) {
        ctx.vertex_array_id = 0;
        glBindVertexArray(0);
        // EBO binding is part of VAO state; with VAO 0 the queried binding is always 0.
        ctx.buffer_target[Buffer::Type::Element] = 0;
    });
}

VertexArray::VertexArray(decltype(attributes)&& attributes, std::shared_ptr<Buffer> element_buffer)
    : attributes(std::move(attributes))
    , element_buffer(std::move(element_buffer))
    , id {0} {
    glGenVertexArrays(1, &id);
    initialize();
}

VertexArray::VertexArray(const VertexArray& rhs)
    : attributes {rhs.attributes}
    , element_buffer {rhs.element_buffer}
    , id {0} {
    glGenVertexArrays(1, &id);
    initialize();
}

VertexArray::~VertexArray() {
    if (id != 0) {
        Context::apply([this] (Context& ctx) {
            if (ctx.vertex_array_id == id) {
                ctx.vertex_array_id = 0;
                ctx.buffer_target[Buffer::Type::Element] = 0;
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
        // GL_ELEMENT_ARRAY_BUFFER is part of VAO state; mirror it from our element_buffer without glGet*.
        ctx.buffer_target[Buffer::Type::Element] = element_buffer ? element_buffer->getId() : 0;
    });
    if (element_buffer) {
        element_buffer->bind();
    }
}

void VertexArray::enable(GLuint index) const noexcept {
    bind();
    glEnableVertexAttribArray(index);
}

void VertexArray::disable(GLuint index) const noexcept {
    bind();
    glDisableVertexAttribArray(index);
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

    attributes.emplace(index, Attribute{size, type, normalized, stride, pointer, buffer});
}

void VertexArray::recreateOnCurrentContext() {
    // std::cout << "Recreating VAO " << id << " on current context" << std::endl;
    glGenVertexArrays(1, &id);
    // std::cout << "New VAO ID: " << id << std::endl;

    for (const auto& [index, attribute] : attributes) {
        const auto& [size, type, normalized, stride, pointer, buffer] = attribute;
        // std::cout << "Setting attribute " << index << " with size " << size << " type " << type << " normalized " << normalized << " stride " << stride << " pointer " << pointer << " buffer " << buffer->getId() << std::endl;
        setAttribute(index, size, type, normalized, stride, pointer, buffer);
    }

    if (element_buffer) {
        setElementBuffer(element_buffer);
    }
}

void VertexArray::setElementBuffer(const std::shared_ptr<Buffer>& buffer) {
    //make sure element buffer is unbound, because we should attach it to active VAO by binding
    //in case its already bound
    buffer->unbind();

    bind();
    buffer->bind();

    // if (!element_buffer || element_buffer->getId() != buffer->getId()) {
        element_buffer = buffer;
    // }
}

void VertexArray::resize(const std::shared_ptr<Buffer>& buffer, size_t size) {
    auto old_id = buffer->getId();

    std::set<GLuint> attrib_ids;
    for (const auto& [index, attribute] : attributes) {
        if (attribute.buffer->getId() == buffer->getId()) {
            attrib_ids.insert(index);
        }
    }

    buffer->resize(size);

    auto new_id = buffer->getId();

    if (old_id != new_id) {
        for (const auto& [id, attribute] : attributes) {
            if (attrib_ids.find(id) != attrib_ids.end()) {
                const auto& [size, type, normalized, stride, pointer, buffer] = attribute;
                setAttribute(id, size, type, normalized, stride, pointer, buffer);
            }
        }
    }
}

VertexArray::Builder VertexArray::builder() {
    return {};
}
