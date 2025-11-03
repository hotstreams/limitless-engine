#include <limitless/core/vertex_stream/vertex_array_builder.hpp>

using namespace Limitless;

VertexArray::Builder& VertexArray::Builder::attribute(GLuint index, const Attribute& attribute) {
    attributes.emplace(index, attribute);
    return *this;
}

VertexArray::Builder& VertexArray::Builder::element_buffer(const std::shared_ptr<Buffer>& buffer) {
    if (buffer->getType() != Buffer::Type::Element) {
        throw std::runtime_error("Buffer should be Element!");
    }
    element_buffer_ = buffer;
    return *this;
}

std::shared_ptr<VertexArray> VertexArray::Builder::build() {
    auto* vertex_array = new VertexArray(std::move(attributes), element_buffer_);
    return std::shared_ptr<VertexArray>(vertex_array);
}