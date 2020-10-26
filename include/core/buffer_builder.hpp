#pragma once

#include <core/context.hpp>
#include <core/named_buffer.hpp>
#include <core/indexed_buffer.hpp>
#include <core/triple_buffer.hpp>

namespace GraphicsEngine {
    class BufferBuilder {
    public:
        template<typename T>
        static std::unique_ptr<Buffer> build(Buffer::Type target, const std::vector<T>& data, Buffer::Usage usage, Buffer::MutableAccess access) noexcept {
            return ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access") ?
                std::make_unique<NamedBuffer>(target, data.size() * sizeof(T), data.data(), usage, access) :
                std::make_unique<StateBuffer>(target, data.size() * sizeof(T), data.data(), usage, access);
        }

        static std::unique_ptr<Buffer> build(Buffer::Type target, size_t bytes, Buffer::Usage usage, Buffer::MutableAccess access) noexcept {
            return ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access") ?
                std::make_unique<NamedBuffer>(target, bytes, nullptr, usage, access) :
                std::make_unique<StateBuffer>(target, bytes, nullptr, usage, access);
        }

        template<typename T>
        static std::unique_ptr<Buffer> build(Buffer::Type target, const std::vector<T>& data, Buffer::Storage flags, Buffer::ImmutableAccess access) {
            if (ContextInitializer::isExtensionSupported("GL_ARB_buffer_storage")) {
                return ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access") ?
                    std::make_unique<NamedBuffer>(target, data.size() * sizeof(T), data.data(), flags, access) :
                    std::make_unique<StateBuffer>(target, data.size() * sizeof(T), data.data(), flags, access);
            }

            return access == Buffer::ImmutableAccess::None ?
                build(target, data, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None) :
                build(target, data, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);
        }

        static std::unique_ptr<Buffer> build(Buffer::Type target, size_t bytes, Buffer::Storage flags, Buffer::ImmutableAccess access) {

            if (ContextInitializer::isExtensionSupported("GL_ARB_buffer_storage")) {
                return ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access") ?
                    std::make_unique<NamedBuffer>(target, bytes, nullptr, flags, access) :
                    std::make_unique<StateBuffer>(target, bytes, nullptr, flags, access);
            }

            return access == Buffer::ImmutableAccess::None ?
                build(target, bytes, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None) :
                build(target, bytes, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);
        }

        template<typename T>
        static std::shared_ptr<Buffer> buildIndexed(std::string_view name, Buffer::Type target, const std::vector<T>& data, Buffer::Storage flags, Buffer::ImmutableAccess access) {
            auto indexed_buffer = std::shared_ptr(build(target, data, flags, access));
            IndexedBuffer::add(name, indexed_buffer);
            return indexed_buffer;
        }

        static std::shared_ptr<Buffer> buildIndexed(std::string_view name, Buffer::Type target, size_t bytes, Buffer::Storage flags, Buffer::ImmutableAccess access) {
            auto indexed_buffer = std::shared_ptr(build(target, bytes, flags, access));
            IndexedBuffer::add(name, indexed_buffer);
            return indexed_buffer;
        }

        template<typename T>
        static std::shared_ptr<Buffer> buildIndexed(std::string_view name, Buffer::Type target, const std::vector<T>& data, Buffer::Usage flags, Buffer::MutableAccess access) noexcept {
            auto indexed_buffer = std::shared_ptr(build(target, data, flags, access));
            IndexedBuffer::add(name, indexed_buffer);
            return indexed_buffer;
        }

        static std::shared_ptr<Buffer> buildIndexed(std::string_view name, Buffer::Type target, size_t bytes, Buffer::Usage flags, Buffer::MutableAccess access) noexcept {
            auto indexed_buffer = std::shared_ptr(build(target, bytes, flags, access));
            IndexedBuffer::add(name, indexed_buffer);
            return indexed_buffer;
        }

        template<typename T>
        static std::unique_ptr<Buffer> buildTriple(Buffer::Type target, const std::vector<T>& data, Buffer::Usage usage, Buffer::MutableAccess access) noexcept {
            std::array<std::shared_ptr<Buffer>, 3> buffers;
            std::fill(buffers.begin(), buffers.end(), build(target, data, usage, access));
            return std::make_unique<TripleBuffer>(std::move(buffers));
        }

        template<typename T>
        static std::unique_ptr<Buffer> buildTriple(Buffer::Type target, const std::vector<T>& data, Buffer::Storage flags, Buffer::ImmutableAccess access) {
            std::array<std::shared_ptr<Buffer>, 3> buffers;
            for (auto& buf : buffers) {
                if (ContextInitializer::isExtensionSupported("GL_ARB_buffer_storage")) {
                    buf = build(target, data, flags, access);
                } else {
                    buf = access == Buffer::ImmutableAccess::None ?
                            build(target, data, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None) :
                            build(target, data, Buffer::Usage::StreamDraw, Buffer::MutableAccess::WriteUnsync);
                }
            }
            return std::make_unique<TripleBuffer>(std::move(buffers));
        }

        template<typename T>
        static std::shared_ptr<Buffer> buildTripleIndexed(std::string_view name, Buffer::Type target, const std::vector<T>& data, Buffer::Usage usage, Buffer::MutableAccess access) noexcept {
            auto indexed_buffer = std::shared_ptr(buildTriple(target, data, usage, access));
            IndexedBuffer::add(name, indexed_buffer);
            return indexed_buffer;
        }

        template<typename T>
        static std::shared_ptr<Buffer> buildTripleIndexed(std::string_view name, Buffer::Type target, const std::vector<T>& data, Buffer::Storage flags, Buffer::ImmutableAccess access) {
            auto indexed_buffer = std::shared_ptr(buildTriple(target, data, flags, access));
            IndexedBuffer::add(name, indexed_buffer);
            return indexed_buffer;
        }
    };
}
