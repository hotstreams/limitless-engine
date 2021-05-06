#include <limitless/core/triple_buffer.hpp>
#include <cstring>
#include <stdexcept>
#include <cstdlib>

using namespace Limitless;

TripleBuffer::TripleBuffer(decltype(buffers) buffers) noexcept
        : buffers(std::move(buffers)), curr_index(0) {

}

void TripleBuffer::mapData(const void* data, size_t data_size) {
    if (data_size == 0) return;

    const auto &buffer = buffers[curr_index];

    if (data_size > buffer->getSize()) {
        throw std::logic_error("Buffer capacity is not enough to map data.");
    }

    auto access = buffer->getAccess();

    if (access.index() == 0) {
        switch (std::get<MutableAccess>(access)) {
            case MutableAccess::None:
                throw std::runtime_error("Static created buffer should not be mapped.");
            case MutableAccess::Write:
                [[fallthrough]];
            case MutableAccess::WriteOrphaning:
                std::memcpy(mapBufferRange(0, data_size), data, data_size);
                unmapBuffer();
                break;
            case MutableAccess::WriteUnsync:
                waitFence();
                std::memcpy(mapBufferRange(0, data_size), data, data_size);
                glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
                break;
        }
    }

    if (access.index() == 1) {
        switch (std::get<ImmutableAccess>(access)) {
            case ImmutableAccess::None:
                throw std::runtime_error("Static created storage cannot be mapped.");
            case ImmutableAccess::WritePersistence:
                waitFence();
                std::memcpy(mapBufferRange(0, data_size), data, data_size);
                glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
                break;
            case ImmutableAccess::WriteCoherent:
                waitFence();
                std::memcpy(mapBufferRange(0, data_size), data, data_size);
                break;
        }
    }
}

void TripleBuffer::clearSubData(GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data) const noexcept {
    buffers[curr_index]->clearSubData(internalformat, offset, size, format, type, data);
}

void TripleBuffer::clearData(GLenum internalformat, GLenum format, GLenum type, const void* data) const noexcept {
    buffers[curr_index]->clearData(internalformat, format, type, data);
}

void TripleBuffer::bufferSubData(GLintptr offset, size_t sub_size, const void* data) const noexcept {
    buffers[curr_index]->bufferSubData(offset, sub_size, data);
}

void TripleBuffer::bindBaseAs(Type target, GLuint index) const noexcept {
    buffers[curr_index]->bindBaseAs(target, index);
}

void TripleBuffer::bindBase(GLuint index) const noexcept {
    buffers[curr_index]->bindBase(index);
}

void TripleBuffer::bindBufferRangeAs(Type target, GLuint index, GLintptr offset) const noexcept {
    buffers[curr_index]->bindBufferRangeAs(target, index, offset);
}

void TripleBuffer::bindBufferRange(GLuint index, GLintptr offset) const noexcept {
    buffers[curr_index]->bindBufferRange(index, offset);
}

void TripleBuffer::bindAs(Type target) const noexcept {
    buffers[curr_index]->bindAs(target);
}

void TripleBuffer::bind() const noexcept {
    buffers[curr_index]->bind();
}

void TripleBuffer::waitFence() noexcept {
    buffers[curr_index]->waitFence();
}

void TripleBuffer::fence() noexcept {
    buffers[curr_index]->fence();

    curr_index = ++curr_index >= 3 ? 0 : curr_index;
}

void* TripleBuffer::mapBufferRange(GLintptr offset, GLsizeiptr size) const {
    return buffers[curr_index]->mapBufferRange(offset, size);
}

void TripleBuffer::unmapBuffer() const noexcept {
    buffers[curr_index]->unmapBuffer();
}

GLuint TripleBuffer::getId() const noexcept {
    return buffers[curr_index]->getId();
}

Buffer::Type TripleBuffer::getType() const noexcept {
    return buffers[curr_index]->getType();
}

size_t TripleBuffer::getSize() const noexcept {
    return buffers[curr_index]->getSize();
}

const std::variant<Buffer::Usage, Buffer::Storage>& TripleBuffer::getUsageFlags() const noexcept {
    return buffers[curr_index]->getUsageFlags();
}

const std::variant<Buffer::MutableAccess, Buffer::ImmutableAccess>& TripleBuffer::getAccess() const noexcept {
    return buffers[curr_index]->getAccess();
}
