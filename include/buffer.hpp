#pragma once

#include <context_debug.hpp>
#include <string_view>
#include <unordered_map>
#include <map>
#include <memory>
#include <variant>
#include <cstring>

namespace GraphicsEngine {
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

        Buffer() noexcept = default;
        virtual ~Buffer() = default;

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

    class StateBuffer : public Buffer {
    protected:
        GLuint id;
        Type target;
        size_t size;
        std::variant<Usage, Storage> usage_flags;
        std::variant<MutableAccess, ImmutableAccess> access;

        // for immutable persistence mapping
        std::optional<void*> persistent_ptr;
        std::optional<GLsync> sync;

        virtual void bufferStorage(const void* data);
        virtual void bufferData(const void* data) const noexcept;

        StateBuffer() noexcept;
        friend void swap(StateBuffer& lhs, StateBuffer& rhs) noexcept;
    public:
        StateBuffer(Type target, size_t size, const void* data, Usage usage, MutableAccess access) noexcept;
        StateBuffer(Type target, size_t size, const void* data, Storage usage, ImmutableAccess access);
        ~StateBuffer() override;

        StateBuffer(const StateBuffer&) noexcept = delete;
        StateBuffer& operator=(const StateBuffer&) const noexcept = delete;

        StateBuffer(StateBuffer&& rhs) noexcept;
        StateBuffer& operator=(StateBuffer&& rhs) noexcept;

        void clearSubData(GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data) const noexcept override;
        void clearData(GLenum internalformat, GLenum format, GLenum type, const void* data) const noexcept override;
        void bufferSubData(GLintptr offset, size_t sub_size, const void* data) const noexcept override;
        void mapData(const void* data, size_t data_size) override;

        void bindBufferRangeAs(Type target, GLuint index, GLintptr offset) const noexcept override;
        void bindBufferRange(GLuint index, GLintptr offset) const noexcept override;
        void bindBaseAs(Type target, GLuint index) const noexcept override;
        void bindBase(GLuint index) const noexcept override;
        void bindAs(Type target) const noexcept override;
        void bind() const noexcept override;

        void fence() noexcept override;
        void waitFence() noexcept override;

        [[nodiscard]] void* mapBufferRange(GLintptr offset, GLsizeiptr size) const override;
        void unmapBuffer() const noexcept override;

        [[nodiscard]] GLuint getId() const noexcept override;
        [[nodiscard]] Type getType() const noexcept override;
        [[nodiscard]] size_t getSize() const noexcept override;
        [[nodiscard]] const std::variant<Usage, Storage>& getUsageFlags() const noexcept override;
        [[nodiscard]] const std::variant<MutableAccess, ImmutableAccess>& getAccess() const noexcept override;
    };

    class NamedBuffer : public StateBuffer {
    private:
        void bufferStorage(const void* data) override;
        void bufferData(const void* data) const noexcept override;
    public:
        NamedBuffer(Type target, size_t size, const void* data, Usage usage, MutableAccess access) noexcept;
        NamedBuffer(Type target, size_t size, const void* data, Storage usage, ImmutableAccess access);
        ~NamedBuffer() override = default;

        NamedBuffer(const NamedBuffer&) noexcept = delete;
        NamedBuffer& operator=(const NamedBuffer&) noexcept = delete;

        NamedBuffer(NamedBuffer&& rhs) noexcept;
        NamedBuffer& operator=(NamedBuffer&& rhs) noexcept;

        void clearData(GLenum internalformat, GLenum format, GLenum type, const void* data) const noexcept override;
        void bufferSubData(GLintptr offset, size_t sub_size, const void* data) const noexcept override;

        [[nodiscard]] void* mapBufferRange(GLintptr offset, GLsizeiptr size) const override;
        void unmapBuffer() const noexcept override;
    };

    class TripleBuffer : public Buffer {
    private:
        std::array<std::shared_ptr<Buffer>, 3> buffers;
        mutable uint8_t curr_index;
    public:
        explicit TripleBuffer(decltype(buffers) buffers) noexcept;

        void clearSubData(GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data) const noexcept override;
        void clearData(GLenum internalformat, GLenum format, GLenum type, const void* data) const noexcept override;
        void bufferSubData(GLintptr offset, size_t sub_size, const void* data) const noexcept override;
        void mapData(const void* data, size_t data_size) override;

        void bindBufferRangeAs(Type target, GLuint index, GLintptr offset) const noexcept override;
        void bindBufferRange(GLuint index, GLintptr offset) const noexcept override;
        void bindBaseAs(Type target, GLuint index) const noexcept override;
        void bindBase(GLuint index) const noexcept override;
        void bindAs(Type target) const noexcept override;
        void bind() const noexcept override;

        void fence() noexcept override;
        void waitFence() noexcept override;

        [[nodiscard]] void* mapBufferRange(GLintptr offset, GLsizeiptr size) const override;
        void unmapBuffer() const noexcept override;

        [[nodiscard]] GLuint getId() const noexcept override;
        [[nodiscard]] Type getType() const noexcept override;
        [[nodiscard]] size_t getSize() const noexcept override;
        [[nodiscard]] const std::variant<Usage, Storage>& getUsageFlags() const noexcept override;
        [[nodiscard]] const std::variant<MutableAccess, ImmutableAccess>& getAccess() const noexcept override;
    };

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

    void swap(StateBuffer& lhs, StateBuffer& rhs) noexcept;
}
