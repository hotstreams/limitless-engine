#pragma once

#include <limitless/core/buffer/buffer.hpp>
#include <optional>

namespace Limitless {
    /**
     * Implementation of Buffer
     *
     * Uses OpenGL function with state changing
     *
     * Two types of buffer: mutable and immutable
     */
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
        /**
         * Mutable buffer constructor
         */
        StateBuffer(Type target, size_t size, const void* data, Usage usage, MutableAccess access) noexcept;
        /**
         * Immutable buffer constructor
         */
        StateBuffer(Type target, size_t size, const void* data, Storage usage, ImmutableAccess access);
        ~StateBuffer() override;

        StateBuffer(const StateBuffer&) noexcept = delete;
        StateBuffer& operator=(const StateBuffer&) noexcept = delete;

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

        StateBuffer* clone() override;

        void resize(size_t size) noexcept override;

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

    void swap(StateBuffer& lhs, StateBuffer& rhs) noexcept;
}
