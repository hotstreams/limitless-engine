#include <core/named_buffer.hpp>
#include <stdexcept>

using namespace LimitlessEngine;

NamedBuffer::NamedBuffer(NamedBuffer&& rhs) noexcept : StateBuffer() {
    swap(*this, rhs);
}

NamedBuffer& NamedBuffer::operator=(NamedBuffer&& rhs) noexcept {
    swap(*this, rhs);
    return *this;
}

NamedBuffer::NamedBuffer(Type _target, size_t _size, const void* data, Usage usage, MutableAccess _access) noexcept {
    target = _target;
    size = _size;
    usage_flags = usage;
    access = _access;

    glCreateBuffers(1, &id);

    NamedBuffer::bufferData(data);
}

NamedBuffer::NamedBuffer(Type _target, size_t _size, const void* data, Storage usage, ImmutableAccess _access) {
    target = _target;
    size = _size;
    usage_flags = usage;
    access = _access;

    glCreateBuffers(1, &id);

    NamedBuffer::bufferStorage(data);
}

void NamedBuffer::bufferStorage(const void* data) {
    auto flags = static_cast<GLenum>(std::get<Storage>(usage_flags));
    auto immutable_access = std::get<ImmutableAccess>(access);

    glNamedBufferStorage(id, size, data, flags);

    if (immutable_access == ImmutableAccess::WritePersistence || immutable_access == ImmutableAccess::WriteCoherent) {
        persistent_ptr = NamedBuffer::mapBufferRange(0, size);
    }
}

void NamedBuffer::bufferData(const void* data) const noexcept {
    glNamedBufferData(id, size, data, static_cast<GLenum>(std::get<Usage>(usage_flags)));
}

void NamedBuffer::bufferSubData(GLintptr offset, size_t sub_size, const void* data) const noexcept {
    glNamedBufferSubData(id, offset, sub_size, data);
}

void NamedBuffer::clearData(GLenum internalformat, GLenum format, GLenum type, const void* data) const noexcept {
    glClearNamedBufferData(id, internalformat, format, type, data);
}

void* NamedBuffer::mapBufferRange(GLintptr offset, GLsizeiptr map_size) const {
    if (persistent_ptr) return persistent_ptr.value();

    auto acc = access.index() ? static_cast<GLenum>(std::get<ImmutableAccess>(access)) : static_cast<GLenum>(std::get<MutableAccess>(access));

    auto* ptr = glMapNamedBufferRange(id, offset, map_size, acc);

    if (!ptr) {
        glUnmapNamedBuffer(id);
        throw std::runtime_error("Buffer cannot get pointer to data.");
    }

    return ptr;
}

void NamedBuffer::unmapBuffer() const noexcept {
    glUnmapNamedBuffer(id);
}