#include <limitless/core/buffer_builder.hpp>

#include <limitless/core/context_initializer.hpp>
#include <limitless/core/context_state.hpp>
#include <limitless/core/named_buffer.hpp>

using namespace LimitlessEngine;

BufferBuilder& BufferBuilder::setTarget(Buffer::Type _target) {
    target = _target;
    return *this;
}

BufferBuilder& BufferBuilder::setUsage(Buffer::Usage _usage) {
    usage = _usage;
    return *this;
}

BufferBuilder& BufferBuilder::setUsage(Buffer::Storage _storage) {
    usage = _storage;
    return *this;
}

BufferBuilder& BufferBuilder::setAccess(Buffer::MutableAccess _access) {
    access = _access;
    return *this;
}

BufferBuilder& BufferBuilder::setAccess(Buffer::ImmutableAccess _access) {
    access = _access;
    return *this;
}

BufferBuilder& BufferBuilder::setData(const void* _data) {
    data = _data;
    return *this;
}

BufferBuilder& BufferBuilder::setDataSize(size_t _size) {
    size = _size;
    return *this;
}

std::unique_ptr<Buffer> BufferBuilder::build() {
    if (ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access")) {
        // building DSA buffer
        if (std::holds_alternative<Buffer::Usage>(usage) && std::holds_alternative<Buffer::MutableAccess>(access)) {
            // building mutable buffer
            return std::make_unique<NamedBuffer>(target, size, data, std::get<Buffer::Usage>(usage), std::get<Buffer::MutableAccess>(access));
        } else {
            // building immutable buffer
            if (ContextInitializer::isExtensionSupported("GL_ARB_buffer_storage")) {
                return std::make_unique<NamedBuffer>(target, size, data, std::get<Buffer::Storage>(usage), std::get<Buffer::ImmutableAccess>(access));
            } else {
                // not supported, building mutable based on access
                return std::get<Buffer::ImmutableAccess>(access) == Buffer::ImmutableAccess::None ?
                       std::make_unique<NamedBuffer>(target, size, data, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None) :
                       std::make_unique<NamedBuffer>(target, size, data, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);
            }
        }
    } else {
        // building state buffer
        if (std::holds_alternative<Buffer::Usage>(usage) && std::holds_alternative<Buffer::MutableAccess>(access)) {
            // building mutable buffer
            return std::make_unique<StateBuffer>(target, size, data, std::get<Buffer::Usage>(usage), std::get<Buffer::MutableAccess>(access));
        } else {
            // building immutable buffer
            if (ContextInitializer::isExtensionSupported("GL_ARB_buffer_storage")) {
                return std::make_unique<StateBuffer>(target, size, data, std::get<Buffer::Storage>(usage), std::get<Buffer::ImmutableAccess>(access));
            } else {
                // not supported, building mutable based on access
                return std::get<Buffer::ImmutableAccess>(access) == Buffer::ImmutableAccess::None ?
                       std::make_unique<StateBuffer>(target, size, data, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None) :
                       std::make_unique<StateBuffer>(target, size, data, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);
            }
        }
    }
}

// builds indexed buffer for specified context
std::shared_ptr<Buffer> BufferBuilder::build(std::string_view name, ContextState& ctx) {
    std::shared_ptr<Buffer> buffer = build();
    ctx.getIndexedBuffers().add(name, buffer);
    return buffer;
}


