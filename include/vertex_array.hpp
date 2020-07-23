#pragma once

#include <vertex.hpp>
#include <buffer.hpp>

namespace GraphicsEngine {
    struct VertexAttribute {
        GLint size;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        const GLvoid* pointer;
        Buffer& buffer;
    };

    class VertexArray {
    private:
        GLuint id;
        GLuint next_attribute_index;

        explicit VertexArray(GLuint id) noexcept;
        friend void swap(VertexArray& lhs, VertexArray& rhs);
    public:
        VertexArray() noexcept;
        ~VertexArray();

        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;

        VertexArray(VertexArray&&) noexcept;
        VertexArray& operator=(VertexArray&&) noexcept;

        void bind() const noexcept;
        void enableAttribute(GLuint index) const noexcept;
        void disableAttribute(GLuint index) const noexcept;
        [[nodiscard]] auto getId() const noexcept { return id; }

        VertexArray& operator<<(const Buffer& element_buffer) noexcept;
        VertexArray& operator<<(const VertexAttribute& attribute) noexcept;
        VertexArray& operator<<(const std::pair<Vertex, Buffer&>& attribute) noexcept;
        VertexArray& operator<<(const std::pair<VertexNormal, Buffer&>& attribute) noexcept;
        VertexArray& operator<<(const std::pair<VertexNormalTangent, Buffer&>& attribute) noexcept;
        VertexArray& operator<<(const std::pair<VertexPackedNormalTangent, Buffer&>& attribute) noexcept;
    };

    void swap(VertexArray& lhs, VertexArray& rhs);
}