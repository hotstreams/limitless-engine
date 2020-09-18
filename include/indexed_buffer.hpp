#pragma once

#include <buffer.hpp>

#include <string>
#include <stdexcept>
#include <unordered_map>
#include <map>
#include <memory>

namespace GraphicsEngine {
    struct buffer_not_found : public std::runtime_error {
        explicit buffer_not_found(const std::string& name) : runtime_error(name) {}
    };

    struct multiple_indexed_buffers : public std::runtime_error {
        explicit multiple_indexed_buffers(const std::string& name) : runtime_error(name) {}
    };

    class IndexedBuffer {
    public:
        enum class Type { UniformBuffer = GL_UNIFORM_BLOCK, ShaderStorage = GL_SHADER_STORAGE_BLOCK };
        using Identifier = std::pair<Type, std::string>;
    private:
        static inline std::unordered_multimap<std::string, std::shared_ptr<Buffer>> buffers;
        static inline std::unordered_map<IndexedBuffer::Type, GLint> current_bind;
        static inline std::map<Identifier, GLuint> bound;
    public:
        static GLuint getBindingPoint(const Identifier& buffer) noexcept;

        static void add(std::string_view name, std::shared_ptr<Buffer> buffer);
        static std::shared_ptr<Buffer> get(std::string_view name);
    };
}