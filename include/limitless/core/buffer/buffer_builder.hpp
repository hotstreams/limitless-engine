#pragma once

#include <limitless/core/buffer/buffer.hpp>
#include <memory>

namespace Limitless {
    class ContextState;

    class Buffer::Builder {
    private:
        Type target_;
        std::variant<Usage, Storage> usage_;
        std::variant<MutableAccess, ImmutableAccess> access_;
        const void* data_ {};
        size_t size_ {};
    public:
        Builder& target(Type _target);
        Builder& usage(Usage _usage);
        Builder& usage(Storage _storage);
        Builder& access(MutableAccess _access);
        Builder& access(ImmutableAccess _access);
        Builder& data(const void* _data);
        Builder& size(size_t _size);

        std::unique_ptr<Buffer> build();
        // builds indexed buffer for specified context
        std::shared_ptr<Buffer> build(std::string_view name, ContextState& ctx);
    };
}
