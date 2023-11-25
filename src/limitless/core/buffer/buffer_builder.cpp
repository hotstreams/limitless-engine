#include <limitless/core/buffer/buffer_builder.hpp>

#include <limitless/core/context_initializer.hpp>
#include <limitless/core/context_state.hpp>
#include <limitless/core/buffer/named_buffer.hpp>

using namespace Limitless;

Buffer::Builder Buffer::builder() {
    return {};
}

Buffer::Builder& Buffer::Builder::target(Buffer::Type _target) {
    target_ = _target;
    return *this;
}

Buffer::Builder& Buffer::Builder::usage(Buffer::Usage _usage) {
    usage_ = _usage;
    return *this;
}

Buffer::Builder& Buffer::Builder::usage(Buffer::Storage _storage) {
    usage_ = _storage;
    return *this;
}

Buffer::Builder& Buffer::Builder::access(Buffer::MutableAccess _access) {
    access_ = _access;
    return *this;
}

Buffer::Builder& Buffer::Builder::access(Buffer::ImmutableAccess _access) {
    access_ = _access;
    return *this;
}

Buffer::Builder& Buffer::Builder::data(const void* _data) {
    data_ = _data;
    return *this;
}

Buffer::Builder& Buffer::Builder::size(size_t _size) {
    size_ = _size;
    return *this;
}

std::unique_ptr<Buffer> Buffer::Builder::build() {
    if (ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access")) {
        // building DSA buffer
        if (std::holds_alternative<Buffer::Usage>(usage_) && std::holds_alternative<Buffer::MutableAccess>(access_)) {
            // building mutable buffer
            return std::make_unique<NamedBuffer>(target_, size_, data_, std::get<Buffer::Usage>(usage_), std::get<Buffer::MutableAccess>(access_));
        } else {
            // building immutable buffer
            if (ContextInitializer::isExtensionSupported("GL_ARB_buffer_storage")) {
                return std::make_unique<NamedBuffer>(target_, size_, data_, std::get<Buffer::Storage>(usage_), std::get<Buffer::ImmutableAccess>(access_));
            } else {
                // not supported, building mutable based on access
                return std::get<Buffer::ImmutableAccess>(access_) == Buffer::ImmutableAccess::None ?
                       std::make_unique<NamedBuffer>(target_, size_, data_, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None) :
                       std::make_unique<NamedBuffer>(target_, size_, data_, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);
            }
        }
    } else {
        // building state buffer
        if (std::holds_alternative<Buffer::Usage>(usage_) && std::holds_alternative<Buffer::MutableAccess>(access_)) {
            // building mutable buffer
            return std::make_unique<StateBuffer>(target_, size_, data_, std::get<Buffer::Usage>(usage_), std::get<Buffer::MutableAccess>(access_));
        } else {
            // building immutable buffer
            if (ContextInitializer::isExtensionSupported("GL_ARB_buffer_storage")) {
                return std::make_unique<StateBuffer>(target_, size_, data_, std::get<Buffer::Storage>(usage_), std::get<Buffer::ImmutableAccess>(access_));
            } else {
                // not supported, building mutable based on access
                return std::get<Buffer::ImmutableAccess>(access_) == Buffer::ImmutableAccess::None ?
                       std::make_unique<StateBuffer>(target_, size_, data_, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None) :
                       std::make_unique<StateBuffer>(target_, size_, data_, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);
            }
        }
    }
}

// builds indexed buffer for specified context
std::shared_ptr<Buffer> Buffer::Builder::build(std::string_view name, ContextState& ctx) {
    std::shared_ptr<Buffer> buffer = build();
    ctx.getIndexedBuffers().add(name, buffer);
    return buffer;
}
