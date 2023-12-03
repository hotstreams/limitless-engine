#pragma once

#include <limitless/core/context_debug.hpp>
#include <stdexcept>
#include <variant>

namespace Limitless {
    struct buffer_error : public std::runtime_error {
        explicit buffer_error(const char* error) noexcept : runtime_error(error) {}
    };

    /**
     * Wraps OpenGL Buffer object
     *
     * Provides interface of OpenGL buffer object functions supported by 3.3 version and some extensions
     *
     * You should check existence of some extension before you use this function
     *
     * All functions change current context state
     */
    class Buffer {
    public:
        /**
         * Describes different types of buffer
         */
        enum class Type {
            Array = GL_ARRAY_BUFFER,
            Element = GL_ELEMENT_ARRAY_BUFFER,
            Uniform = GL_UNIFORM_BUFFER,
            ShaderStorage = GL_SHADER_STORAGE_BUFFER,
            AtomicCounter = GL_ATOMIC_COUNTER_BUFFER,
            IndirectDraw = GL_DRAW_INDIRECT_BUFFER,
            IndirectDispatch = GL_DISPATCH_INDIRECT_BUFFER
        };

        /**
         * Describes different usage modes of buffer
         */
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

        /**
         * Describes immutable storage access pattern flags
         */
        enum class Storage {
            Static = GL_NONE,
            Dynamic = GL_DYNAMIC_STORAGE_BIT,
            Client = GL_CLIENT_STORAGE_BIT,
            DynamicWrite = Dynamic | GL_MAP_WRITE_BIT,
            DynamicPersistenceWrite = DynamicWrite | GL_MAP_PERSISTENT_BIT,
            DynamicCoherentWrite = DynamicPersistenceWrite | GL_MAP_COHERENT_BIT
        };

        /**
         *  Describes mutable buffer access
         */
        enum class MutableAccess {
            None,
            Write = GL_MAP_WRITE_BIT,
            WriteOrphaning = Write | GL_MAP_INVALIDATE_BUFFER_BIT,
            WriteUnsync = Write | GL_MAP_UNSYNCHRONIZED_BIT
        };

        /**
         *  Describes immutable buffer access
         */
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

        /**
         * Wraps glClearBufferSubData() call
         *
         * fills all or part of buffer object's data store with a fixed value
         */
        virtual void clearSubData(GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data) const noexcept = 0;

        /**
         * Wraps glClearBufferData() call
         *
         * fill a buffer object's data store with a fixed value
         */
        virtual void clearData(GLenum internalformat, GLenum format, GLenum type, const void* data) const noexcept = 0;

        /**
         * Wraps glBufferSubData() call
         *
         * updates a subset of a buffer object's data store
         *
         */
        virtual void bufferSubData(GLintptr offset, size_t sub_size, const void* data) const noexcept = 0;

        /**
         * Wraps glMapBufferRange() call
         *
         * map all or part of a buffer object's data store into the client's address space
         */
        virtual void mapData(const void* data, size_t data_size) = 0;

        /**
         * Wraps glBindBufferRange() call
         *
         * bind a range within a buffer object to an indexed buffer target with specified target
         */
        virtual void bindBufferRangeAs(Type target, GLuint index, GLintptr offset) const noexcept = 0;

        /**
         * Wraps glBindBufferRange() call
         *
         * bind a range within a buffer object to an indexed buffer target
         */
        virtual void bindBufferRange(GLuint index, GLintptr offset) const noexcept = 0;

        /**
         * Wraps glBindBufferBase() call
         *
         * bind a buffer object to an indexed buffer target with specified target
         */
        virtual void bindBaseAs(Type target, GLuint index) const noexcept = 0;

        /**
         * Wraps glBindBufferBase() call
         *
         * bind a buffer object to an indexed buffer target
         */
        virtual void bindBase(GLuint index) const noexcept = 0;

        /**
         * Wraps glBindBuffer() call
         *
         * bind a named buffer object with target
         */
        virtual void bindAs(Type target) const noexcept = 0;

        /**
         * Wraps glBindBuffer() call
         *
         * bind a named buffer object
         */
        virtual void bind() const noexcept = 0;

        /**
         * Makes a copy of the buffer without the content
         */
        virtual Buffer* clone() = 0;

        /**
         * Resizes buffer
         *
         * If it is mutable object it is resized
         * If it is immutable object it is recreated
         *
         * Content of the buffer is lost
         */
        virtual void resize(size_t bytes) noexcept = 0;

        /**
         * places glFenceSync for gpu command completion
         */
        virtual void fence() noexcept = 0;

        /**
         * Waits for gpu commands to complete
         */
        virtual void waitFence() noexcept = 0;

        /**
         * Wraps glMapBufferRange  call
         *
         * map a section of a buffer object's data store
         */
        [[nodiscard]] virtual void* mapBufferRange(GLintptr offset, GLsizeiptr size) const = 0;

        /**
         * Wraps glMapBufferRange  call
         *
         * release the mapping of a buffer object's data store into the client's address space
         */
        virtual void unmapBuffer() const noexcept = 0;

        [[nodiscard]] virtual GLuint getId() const noexcept = 0;
        [[nodiscard]] virtual Type getType() const noexcept = 0;
        [[nodiscard]] virtual size_t getSize() const noexcept = 0;
        [[nodiscard]] virtual const std::variant<Usage, Storage>& getUsageFlags() const noexcept = 0;
        [[nodiscard]] virtual const std::variant<MutableAccess, ImmutableAccess>& getAccess() const noexcept = 0;

        class Builder;

        static Builder builder();
    };
}
