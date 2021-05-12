#include <limitless/core/state_buffer.hpp>
#include <limitless/core/context_state.hpp>
#include <stdexcept>
#include <algorithm>
#include <cstring>

using namespace Limitless;

StateBuffer::StateBuffer() noexcept
    : id{0}, target{Type::Array}, size{0}, usage_flags{Usage::StaticDraw}, access{MutableAccess::WriteOrphaning} { }

StateBuffer::StateBuffer(Type target, size_t size, const void* data, Usage usage, MutableAccess access) noexcept
    : id{0}, target{target}, size{size}, usage_flags{usage}, access{access} {
    glGenBuffers(1, &id);

    StateBuffer::bufferData(data);
}

StateBuffer::StateBuffer(Type target, size_t size, const void* data, Storage usage, ImmutableAccess access)
    : id{0}, target{target}, size{size}, usage_flags{usage}, access{access} {
    glGenBuffers(1, &id);

    StateBuffer::bufferStorage(data);
}

StateBuffer::~StateBuffer() {
    if (id != 0) {
        if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
            auto& target_map = state->buffer_target;
            auto& point_map = state->buffer_point;

            std::for_each(target_map.begin(), target_map.end(), [&] (auto& state_target) {
                auto& [s_target, s_id] = state_target;
                if (s_id == id) s_id = 0;
            });

            std::for_each(point_map.begin(), point_map.end(), [&] (auto& state_point) {
                auto& [s_point, s_id] = state_point;
                if (s_id == id) s_id = 0;
            });

            glDeleteBuffers(1, &id);
        }
    }
}

void Limitless::swap(StateBuffer& lhs, StateBuffer& rhs) noexcept {
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
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        if (state->buffer_target[target] != id) {
            glBindBuffer(static_cast<GLenum>(target), id);
            state->buffer_target[target] = id;
        }
    }
}

void StateBuffer::bindAs(Type _target) const noexcept {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        if (state->buffer_target[_target] != id) {
            glBindBuffer(static_cast<GLenum>(_target), id);
            state->buffer_target[_target] = id;
        }
    }
}

void StateBuffer::bindBaseAs(Type _target, GLuint index) const noexcept {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        auto& point_map = state->buffer_point;
        auto& target_map = state->buffer_target;
        if (point_map[{_target, index}] != id) {
            glBindBufferBase(static_cast<GLenum>(_target), index, id);
            point_map[{_target, index}] = id;
            target_map[_target] = id;
        }
    }
}

void StateBuffer::bindBase(GLuint index) const noexcept {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        auto& point_map = state->buffer_point;
        auto& target_map = state->buffer_target;
        if (point_map[{target, index}] != id) {
            glBindBufferBase(static_cast<GLenum>(target), index, id);
            point_map[{target, index}] = id;
            target_map[target] = id;
        }
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
        throw buffer_error{"Buffer capacity is not enough to map data"};
    }

    if (access.index() == 0) {
        switch (std::get<MutableAccess>(access)) {
            case MutableAccess::None:
                throw buffer_error{"Static created buffer should not be mapped"};
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
                throw buffer_error{"Static created storage cannot be mapped"};
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
    if (!ptr) {
        unmapBuffer();
        throw buffer_error{"Buffer cannot get pointer to data."};
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
        if (auto acc = std::get<ImmutableAccess>(access); acc == ImmutableAccess::WritePersistence || acc == ImmutableAccess::WriteCoherent) {
            sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        }
    }
}

void StateBuffer::bindBufferRangeAs(Buffer::Type _target, GLuint index, GLintptr offset) const noexcept {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        if (auto& point_map = state->buffer_point; point_map[{_target, index}] != id) {
            glBindBufferRange(static_cast<GLenum>(_target), index, id, offset, size);
            point_map[{_target, index}] = id;
        }
    }
}

void StateBuffer::bindBufferRange(GLuint index, GLintptr offset) const noexcept {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        if (auto& point_map = state->buffer_point; point_map[{target, index}] != id) {
            glBindBufferRange(static_cast<GLenum>(target), index, id, offset, size);
            point_map[{target, index}] = id;
        }
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

void StateBuffer::resize(size_t new_size) noexcept{
    size = new_size;

    if (std::holds_alternative<Storage>(usage_flags)) {
        StateBuffer new_buffer{target, size, nullptr, std::get<Storage>(usage_flags), std::get<ImmutableAccess>(access)};
        swap(*this, new_buffer);
    } else {
        bind();
        StateBuffer::bufferData(nullptr);
    }
}
