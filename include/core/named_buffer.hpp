#pragma once

#include <core/state_buffer.hpp>

namespace GraphicsEngine {
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
}
