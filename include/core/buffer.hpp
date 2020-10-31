#pragma once

#include <core/context_debug.hpp>
#include <stdexcept>
#include <variant>

namespace GraphicsEngine {
    struct buffer_error : public std::runtime_error {
        explicit buffer_error(const char* error) noexcept : runtime_error(error) {}
    };

    class Buffer {
    public:
        enum class Type {
            Array = GL_ARRAY_BUFFER,
            Element = GL_ELEMENT_ARRAY_BUFFER,
            Uniform = GL_UNIFORM_BUFFER,
            ShaderStorage = GL_SHADER_STORAGE_BUFFER,
            AtomicCounter = GL_ATOMIC_COUNTER_BUFFER,
            IndirectDraw = GL_DRAW_INDIRECT_BUFFER,
            IndirectDispatch = GL_DISPATCH_INDIRECT_BUFFER
        };

        enum class Usage {
            StaticDraw = GL_STATIC_DRAW,
            StaticRead = GL_STATIC_READ,
            StaticCopy = GL_STATIC_COPY,
            DynamicDraw = GL_DYNAMIC_DRAW,
            DynamicRead = GL_DYNAMIC_READ,
            DynamicCopy = GL_DYNAMIC_COPY,
            StreamDraw = GL_STREAM_DRAW,
            StreamRead = GL_STREAM_READ,
            StreamCopy = GL_STREAM_COPY
        };

        enum class Storage {
            Static = GL_NONE,
            Dynamic = GL_DYNAMIC_STORAGE_BIT,
            Client = GL_CLIENT_STORAGE_BIT,
            DynamicWrite = Dynamic | GL_MAP_WRITE_BIT,
            DynamicPersistenceWrite = DynamicWrite | GL_MAP_PERSISTENT_BIT,
            DynamicCoherentWrite = DynamicPersistenceWrite | GL_MAP_COHERENT_BIT
        };

        enum class MutableAccess {
            None,
            Write = GL_MAP_WRITE_BIT,
            WriteOrphaning = Write | GL_MAP_INVALIDATE_BUFFER_BIT,
            WriteUnsync = Write | GL_MAP_UNSYNCHRONIZED_BIT
        };

        enum class ImmutableAccess {
            None,
            WritePersistence = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT,
            WriteCoherent = WritePersistence | GL_MAP_COHERENT_BIT
        };

        Buffer() = default;
        virtual ~Buffer() = default;

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        Buffer(Buffer&&) noexcept = default;
        Buffer& operator=(Buffer&&) noexcept = default;

        virtual void clearSubData(GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data) const noexcept = 0;
        virtual void clearData(GLenum internalformat, GLenum format, GLenum type, const void* data) const noexcept = 0;
        virtual void bufferSubData(GLintptr offset, size_t sub_size, const void* data) const noexcept = 0;
        virtual void mapData(const void* data, size_t data_size) = 0;

        virtual void bindBufferRangeAs(Type target, GLuint index, GLintptr offset) const noexcept = 0;
        virtual void bindBufferRange(GLuint index, GLintptr offset) const noexcept = 0;
        virtual void bindBaseAs(Type target, GLuint index) const noexcept = 0;
        virtual void bindBase(GLuint index) const noexcept = 0;
        virtual void bindAs(Type target) const noexcept = 0;
        virtual void bind() const noexcept = 0;

        virtual void fence() noexcept = 0;
        virtual void waitFence() noexcept = 0;

        [[nodiscard]] virtual void* mapBufferRange(GLintptr offset, GLsizeiptr size) const = 0;
        virtual void unmapBuffer() const noexcept = 0;

        [[nodiscard]] virtual GLuint getId() const noexcept = 0;
        [[nodiscard]] virtual Type getType() const noexcept = 0;
        [[nodiscard]] virtual size_t getSize() const noexcept = 0;
        [[nodiscard]] virtual const std::variant<Usage, Storage>& getUsageFlags() const noexcept = 0;
        [[nodiscard]] virtual const std::variant<MutableAccess, ImmutableAccess>& getAccess() const noexcept = 0;
    };
}
