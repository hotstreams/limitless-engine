#pragma once

#include <limitless/core/buffer.hpp>
#include <memory>

namespace LimitlessEngine {
    class ContextState;

    class BufferBuilder {
    private:
        Buffer::Type target;
        std::variant<Buffer::Usage, Buffer::Storage> usage;
        std::variant<Buffer::MutableAccess, Buffer::ImmutableAccess> access;
        const void* data {};
        size_t size {};
    public:
        BufferBuilder& setTarget(Buffer::Type _target);
        BufferBuilder& setUsage(Buffer::Usage _usage);
        BufferBuilder& setUsage(Buffer::Storage _storage);
        BufferBuilder& setAccess(Buffer::MutableAccess _access);
        BufferBuilder& setAccess(Buffer::ImmutableAccess _access);
        BufferBuilder& setData(const void* _data);
        BufferBuilder& setDataSize(size_t _size);

        std::unique_ptr<Buffer> build();
        // builds indexed buffer for specified context
        std::shared_ptr<Buffer> build(std::string_view name, ContextState& ctx);
    };
}
