#pragma once

#include <limitless/core/context_debug.hpp>
#include <limitless/core/buffer/buffer.hpp>

#include <map>
#include <functional>
#include <memory>

namespace Limitless {
    class VertexArray {
    public:
        struct Attribute {
            GLint size;
            GLenum type;
            GLboolean normalized;
            GLsizei stride;
            const GLvoid* pointer;
            std::shared_ptr<Buffer> buffer;

            Attribute(GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer, std::shared_ptr<Buffer> buffer) noexcept;
        };

        class Builder;
    private:
        std::map<GLuint, Attribute> attributes;
        std::shared_ptr<Buffer> element_buffer;
        GLuint id;

        void initialize();

        VertexArray(decltype(attributes)&& attributes, std::shared_ptr<Buffer> element_buffer);
    public:
        ~VertexArray();

        VertexArray(const VertexArray&);
        VertexArray& operator=(const VertexArray&);

        VertexArray(VertexArray&&) = default;
        VertexArray& operator=(VertexArray&&) = default;

        [[nodiscard]] auto getId() const noexcept { return id; }
        const auto& getAttributes() const noexcept { return attributes; }

        void bind() const noexcept;

        void enable(GLuint index) const noexcept;
        void disable(GLuint index) const noexcept;

        void setAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer, const std::shared_ptr<Buffer>& buffer);
        void setElementBuffer(const std::shared_ptr<Buffer>& buffer);

        void resize(const std::shared_ptr<Buffer>& buffer, size_t size);

        static Builder builder();
    };
}