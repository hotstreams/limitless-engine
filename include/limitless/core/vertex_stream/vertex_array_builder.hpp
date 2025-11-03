#pragma once

#include <limitless/core/vertex_stream/vertex_array.hpp>

namespace Limitless {
    class VertexArray::Builder {
    private:
        std::map<GLuint, Attribute> attributes;
        std::shared_ptr<Buffer> element_buffer_;
    public:
        Builder& attribute(GLuint index, const Attribute& attribute);
        Builder& element_buffer(const std::shared_ptr<Buffer>& buffer);
        std::shared_ptr<VertexArray> build();
    };
}