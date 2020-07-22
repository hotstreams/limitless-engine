#include <buffer.hpp>
#include <context_state.hpp>
#include <cstring>
#include <context_initializer.hpp>
#include <iostream>

using namespace GraphicsEngine;

StateBuffer::StateBuffer() noexcept : id(0), target(Type::Array), size(0), usage_flags(Usage::StaticDraw), access(MutableAccess::WriteOrphaning) {}

StateBuffer::StateBuffer(Type target, size_t size, const void* data, Usage usage, MutableAccess access) noexcept
    : id(0), target(target), size(size), usage_flags(usage), access(access) {

    glGenBuffers(1, &id);

    StateBuffer::bufferData(data);
}

StateBuffer::StateBuffer(Type target, size_t size, const void* data, Storage usage, ImmutableAccess access)
    : id(0), target(target), size(size), usage_flags(usage), access(access) {

    glGenBuffers(1, &id);

    StateBuffer::bufferStorage(data);
}

StateBuffer::~StateBuffer() {
    if (id != 0) {
        glDeleteBuffers(1, &id);
    }
}

void GraphicsEngine::swap(StateBuffer& lhs, StateBuffer& rhs) noexcept {
    using std::swap;

    swap(lhs.id, rhs.id);
    swap(lhs.target, rhs.target);
    swap(lhs.size, rhs.size);
    swap(lhs.usage_flags, rhs.usage_flags);
    swap(lhs.access, rhs.access);
    swap(lhs.persistent_ptr, rhs.persistent_ptr);
    swap(lhs.sync, rhs.sync);
}

StateBuffer::StateBuffer(StateBuffer&& rhs) noexcept : StateBuffer() {
    swap(*this, rhs);
}

StateBuffer& StateBuffer::operator=(StateBuffer&& rhs) noexcept {
    swap(*this, rhs);
    return *this;
}

void StateBuffer::bind() const noexcept {
    auto& target_map = ContextState::getState(glfwGetCurrentContext()).buffer_target;
    if (target_map[target] != id) {
        glBindBuffer(static_cast<GLenum>(target), id);
        target_map[target] = id;
    }
}

void StateBuffer::bindAs(Type _target) const noexcept {
    auto& target_map = ContextState::getState(glfwGetCurrentContext()).buffer_target;
    if (target_map[_target] != id) {
        glBindBuffer(static_cast<GLenum>(_target), id);
        target_map[_target] = id;
    }
}

void StateBuffer::bindBaseAs(Type _target, GLuint index) const noexcept {
    auto& point_map = ContextState::getState(glfwGetCurrentContext()).buffer_point;
    auto& target_map = ContextState::getState(glfwGetCurrentContext()).buffer_target;
    if (point_map[{ _target, index }] != id) {
        glBindBufferBase(static_cast<GLenum>(_target), index, id);
        point_map[{ _target, index }] = id;
        target_map[_target] = id;
    }
}

void StateBuffer::bindBase(GLuint index) const noexcept {
    auto& point_map = ContextState::getState(glfwGetCurrentContext()).buffer_point;
    auto& target_map = ContextState::getState(glfwGetCurrentContext()).buffer_target;
    if (point_map[{ target, index }] != id) {
        glBindBufferBase(static_cast<GLenum>(target), index, id);
        point_map[{ target, index }] = id;
        target_map[target] = id;
    }
}

void StateBuffer::bufferData(const void* data) const noexcept {
    bind();

    glBufferData(static_cast<GLenum>(target), size, data, static_cast<GLenum>(std::get<Usage>(usage_flags)));
}

void StateBuffer::bufferStorage(const void* data) {
    bind();

    auto flags = static_cast<GLenum>(std::get<Storage>(usage_flags));
    auto immutable_access = std::get<ImmutableAccess>(access);

    glBufferStorage(static_cast<GLenum>(target), size, data, flags);

    if (immutable_access == ImmutableAccess::WritePersistence || immutable_access == ImmutableAccess::WriteCoherent) {
        persistent_ptr = StateBuffer::mapBufferRange(0, size);
    }
}

void StateBuffer::bufferSubData(GLintptr offset, size_t sub_size, const void* data) const noexcept {
    bind();

    glBufferSubData(static_cast<GLenum>(target), offset, sub_size, data);
}

void StateBuffer::clearData(GLenum internalformat, GLenum format, GLenum type, const void* data) const noexcept {
    bind();

    glClearBufferData(static_cast<GLenum>(target), internalformat, format, type, data);
}

void StateBuffer::waitFence() noexcept {
    if (sync) {
        for (;;) {
            auto wait_result = glClientWaitSync(sync.value(), 0, 0);
            if (wait_result == GL_ALREADY_SIGNALED || wait_result == GL_CONDITION_SATISFIED)
                break;
        }
        glDeleteSync(sync.value());
        sync.reset();
    }
}

void StateBuffer::mapData(const void* data, size_t data_size) {
    if (data_size == 0) return;

    if (data_size > size) {
        throw std::logic_error("Buffer capacity is not enough to map data.");
    }

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
                    unmapBuffer();
                    break;
            }
    }

    if (access.index() == 1) {
        switch (std::get<ImmutableAccess>(access)) {
            case ImmutableAccess::None:
                throw std::runtime_error("Static created storage cannot be mapped.");
            case ImmutableAccess::WritePersistence:
                waitFence();
                std::memcpy(persistent_ptr.value(), data, data_size);
                glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
                break;
            case ImmutableAccess::WriteCoherent:
                waitFence();
                std::memcpy(persistent_ptr.value(), data, data_size);
                break;
        }
    }
}

GLuint StateBuffer::getId() const noexcept {
    return id;
}

void* StateBuffer::mapBufferRange(GLintptr offset, GLsizeiptr map_size) const {
    if (persistent_ptr) return persistent_ptr.value();

    auto acc = access.index() ? static_cast<GLenum>(std::get<ImmutableAccess>(access)) : static_cast<GLenum>(std::get<MutableAccess>(access));

    bind();

    auto* ptr = glMapBufferRange(static_cast<GLenum>(target), offset, map_size, acc);
    int bind;
    glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &bind);
    if (!ptr) {
        unmapBuffer();
        throw std::runtime_error("Buffer cannot get pointer to data.");
    }

    return ptr;
}

void StateBuffer::unmapBuffer() const noexcept {
    bind();

    glUnmapBuffer(static_cast<GLenum>(target));
}

void StateBuffer::fence() noexcept {
    if (sync) {
        glDeleteSync(sync.value());
        sync.reset();
    }

    if (access.index() == 0) {
        if (std::get<MutableAccess>(access) == MutableAccess::WriteUnsync) {
            sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        }
    }

    if (access.index() == 1) {
        auto acc = std::get<ImmutableAccess>(access);
        if (acc == ImmutableAccess::WritePersistence || acc == ImmutableAccess::WriteCoherent) {
            sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        }
    }
}

void StateBuffer::bindBufferRangeAs(Buffer::Type _target, GLuint index, GLintptr offset) const noexcept {
    auto& point_map = ContextState::getState(glfwGetCurrentContext()).buffer_point;
    if (point_map[{ _target, index }] != id) {
        glBindBufferRange(static_cast<GLenum>(_target), index, id, offset, size);
        point_map[{ _target, index }] = id;
    }
}

void StateBuffer::bindBufferRange(GLuint index, GLintptr offset) const noexcept {
    auto& point_map = ContextState::getState(glfwGetCurrentContext()).buffer_point;
    if (point_map[{ target, index }] != id) {
        glBindBufferRange(static_cast<GLenum>(target), index, id, offset, size);
        point_map[{ target, index }] = id;
    }
}

Buffer::Type StateBuffer::getType() const noexcept {
    return target;
}

size_t StateBuffer::getSize() const noexcept {
    return size;
}

const std::variant<Buffer::Usage, Buffer::Storage>& StateBuffer::getUsageFlags() const noexcept {
    return usage_flags;
}

const std::variant<Buffer::MutableAccess, Buffer::ImmutableAccess>& StateBuffer::getAccess() const noexcept {
    return access;
}

void StateBuffer::clearSubData(GLenum internalformat, GLintptr offset, GLsizeiptr sub_size, GLenum format, GLenum type, const void *data) const noexcept {
    bind();

    glClearBufferSubData(static_cast<GLenum>(target), internalformat, offset, sub_size, format, type, data);
}

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

std::shared_ptr<Buffer> IndexedBuffer::get(std::string_view name) {
    auto count = buffers.count(name.data());

    if (count == 0) {
        throw buffer_not_found("Failed to find IndexedBuffer with name " + std::string(name.data()));
    }

    if (count == 1) {
        return buffers.find(name.data())->second;
    }

    throw multiple_indexed_buffers("More than one buffer, should be set manually which one");
}

void IndexedBuffer::add(std::string_view name, std::shared_ptr<Buffer> buffer) {
    buffers.emplace(name, std::move(buffer));
}

GLuint IndexedBuffer::getBindingPoint(const Identifier& buffer) noexcept {
    const auto& [type, name] = buffer;

    auto point_bound = bound.find(buffer);
    if (point_bound != bound.end()) {
        return point_bound->second;
    }

    GLuint bind;
    auto found = current_bind.find(type);
    if (found == current_bind.end()) {
        bind = 0;
        current_bind.emplace(type, bind);
    } else {
        auto& next_bind = ++found->second;
        switch (type) {
            case IndexedBuffer::Type::UniformBuffer:
                if (next_bind >= ContextInitializer::limits.uniform_buffer_max_count) {
                    next_bind = 0;
                }
                break;
            case IndexedBuffer::Type::ShaderStorage:
                if (next_bind >= ContextInitializer::limits.shader_storage_max_count) {
                    next_bind = 0;
                }
                break;
        }
        bind = next_bind;
    }

    bound.emplace(buffer, bind);

    return bind;
}
