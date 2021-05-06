#pragma once

#include <limitless/core/context_debug.hpp>

#include <unordered_map>
#include <stdexcept>
#include <string>
#include <memory>
#include <mutex>
#include <map>

namespace Limitless {
    class Buffer;

    struct buffer_not_found : public std::runtime_error {
        explicit buffer_not_found(const std::string& name) : runtime_error(name) {}
    };

    class IndexedBuffer final {
    public:
        enum class Type { UniformBuffer = GL_UNIFORM_BLOCK, ShaderStorage = GL_SHADER_STORAGE_BLOCK };
        using Identifier = std::pair<Type, std::string>;
    private:
        std::unordered_multimap<std::string, std::shared_ptr<Buffer>> buffers;
        std::unordered_map<Type, GLint> current_bind;
        std::map<Identifier, GLuint> bound;
    public:
        GLuint getBindingPoint(Type type, std::string_view name) noexcept;

        void add(std::string_view name, std::shared_ptr<Buffer> buffer) noexcept;
        void remove(const std::string& name, const std::shared_ptr<Buffer>& buffer);
        std::shared_ptr<Buffer> get(std::string_view name);
    };

    struct IndexedBufferData {
        IndexedBuffer::Type target;
        std::string name;
        GLuint block_index;
        GLuint bound_point;
        bool index_connected {};

        IndexedBufferData(IndexedBuffer::Type _target, std::string _name, GLuint _block_index, GLuint _bound_point)
                : target{_target}, name{std::move(_name)}, block_index{_block_index}, bound_point{_bound_point} {}
    };
}