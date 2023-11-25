#pragma once

#include <limitless/core/buffer/buffer.hpp>
#include <memory>

namespace Limitless {
    class ContextState;

    class Buffer::Builder {
    private:
        Buffer::Type target_;
        std::variant<Buffer::Usage, Buffer::Storage> usage_;
        std::variant<Buffer::MutableAccess, Buffer::ImmutableAccess> access_;
        const void* data_ {};
        size_t size_ {};
    public:
        Builder& target(Buffer::Type _target);
        Builder& usage(Buffer::Usage _usage);
        Builder& usage(Buffer::Storage _storage);
        Builder& access(Buffer::MutableAccess _access);
        Builder& access(Buffer::ImmutableAccess _access);
        Builder& data(const void* _data);
        Builder& size(size_t _size);

        std::unique_ptr<Buffer> build();
        // builds indexed buffer for specified context
        std::shared_ptr<Buffer> build(std::string_view name, ContextState& ctx);
    };
}
