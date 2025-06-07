#pragma once

#include <limitless/core/vertex.hpp>
#include <limitless/core/context_debug.hpp>
#include <unordered_map>
#include <functional>
#include <memory>

namespace Limitless {
    class Buffer;

    class VertexAttribute {
    public:
        GLint size;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        const GLvoid* pointer;
        std::shared_ptr<Buffer> buffer;

        VertexAttribute(GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer, std::shared_ptr<Buffer> buffer) noexcept;
        ~VertexAttribute() = default;
    };

    class VertexArray {
    private:
        std::unordered_map<GLuint, VertexAttribute> attributes;
        std::shared_ptr<Buffer> element_buffer {};
        GLuint id {};

        friend void swap(VertexArray& lhs, VertexArray& rhs);
    public:
        VertexArray() noexcept;
        ~VertexArray();

        VertexArray(const VertexArray&);
        VertexArray& operator=(const VertexArray&) = delete;

        VertexArray(VertexArray&&) noexcept;
        VertexArray& operator=(VertexArray&&) noexcept;

        [[nodiscard]] auto getId() const noexcept { return id; }

        void bind() const noexcept;

        void enable(GLuint index) const noexcept;
        void disable(GLuint index) const noexcept;

        void setAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer, const std::shared_ptr<Buffer>& buffer);
        void setElementBuffer(const std::shared_ptr<Buffer>& buffer);

        template <typename Attribute>
        VertexArray& setAttribute(GLuint index, bool normalized, GLsizei stride, const GLvoid* pointer, const std::shared_ptr<Buffer>& buffer) {
            if constexpr (std::is_same_v<Attribute, float>) {
                setAttribute(index, 1, GL_FLOAT, normalized, stride, pointer, buffer);
            } else if constexpr (std::is_same_v<Attribute, int>) {
                setAttribute(index, 1, GL_INT, normalized, stride, pointer, buffer);
            } else if constexpr (std::is_same_v<Attribute, uint32_t>) {
                setAttribute(index, 1, GL_UNSIGNED_INT, normalized, stride, pointer, buffer);
            } else if constexpr (std::is_same_v<Attribute, glm::ivec4>) {
	            setAttribute(index, 4, GL_INT, normalized, stride, pointer, buffer);
            } else if constexpr (std::is_same_v<Attribute, glm::vec4>) {
                setAttribute(index, 4, GL_FLOAT, normalized, stride, pointer, buffer);
            } else if constexpr (std::is_same_v<Attribute, glm::vec3>) {
                setAttribute(index, 3, GL_FLOAT, normalized, stride, pointer, buffer);
            } else if constexpr (std::is_same_v<Attribute, glm::vec2>) {
                setAttribute(index, 2, GL_FLOAT, normalized, stride, pointer, buffer);
            } else {
                static_assert(!std::is_same_v<Attribute, Attribute>, "This typeof Attribute cannot be used");
            }
            return *this;
        }

        VertexArray& operator<<(const std::pair<Vertex, const std::shared_ptr<Buffer>&>& attribute) noexcept;
        VertexArray& operator<<(const std::pair<TextVertex, const std::shared_ptr<Buffer>&>& attribute) noexcept;
        VertexArray& operator<<(const std::pair<VertexNormalTangent, const std::shared_ptr<Buffer>&>& attribute) noexcept;
        VertexArray& operator<<(const std::pair<VertexTerrain, const std::shared_ptr<Buffer>&>& attribute) noexcept;
        VertexArray& operator<<(const std::pair<TextSelectionVertex, const std::shared_ptr<Buffer>&>& attribute) noexcept;
    };

    void swap(VertexArray& lhs, VertexArray& rhs);
}