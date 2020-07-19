#pragma once

#include <context.hpp>
#include <buffer.hpp>

namespace GraphicsEngine {
    class BufferBuilder {
    public:
        template<typename T>
        static std::unique_ptr<Buffer> build(Buffer::Type target, const std::vector<T>& data, Buffer::Usage usage, Buffer::MutableAccess access) noexcept {
            auto size = data.size() * sizeof(T);

            if (ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access")) {
                return std::make_unique<NamedBuffer>(target, size, data.data(), usage, access);
            }

            return std::make_unique<StateBuffer>(target, size, data.data(), usage, access);
        }

        template<typename T>
        static std::unique_ptr<Buffer> build(Buffer::Type target, const std::vector<T>& data, Buffer::Storage flags, Buffer::ImmutableAccess access) {
            auto size = data.size() * sizeof(T);
            auto immutable_storage = ContextInitializer::isExtensionSupported("GL_ARB_buffer_storage");

            if (immutable_storage) {
                if (ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access")) {
                    return std::make_unique<NamedBuffer>(target, size, data.data(), flags, access);
                }

                return std::make_unique<StateBuffer>(target, size, data.data(), flags, access);
            }

            if (access == Buffer::ImmutableAccess::None) {
                return build(target, data, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None);
            } else {
                return build(target, data, Buffer::Usage::StreamDraw, Buffer::MutableAccess::WriteOrphaning);
            }
        }

        template<typename T>
        static std::shared_ptr<Buffer> buildIndexed(std::string_view name, Buffer::Type target, const std::vector<T>& data, Buffer::Storage flags, Buffer::ImmutableAccess access) {
            auto indexed_buffer = std::shared_ptr(build(target, data, flags, access));

            IndexedBuffer::add(name, indexed_buffer);

            return indexed_buffer;
        }

        template<typename T>
        static std::shared_ptr<Buffer> buildIndexed(std::string_view name, Buffer::Type target, const std::vector<T>& data, Buffer::Usage flags, Buffer::MutableAccess access) noexcept {
            auto indexed_buffer = std::shared_ptr(build(target, data, flags, access));

            IndexedBuffer::add(name, indexed_buffer);

            return indexed_buffer;
        }

        template<typename T>
        static std::unique_ptr<Buffer> buildTriple(Buffer::Type target, const std::vector<T>& data, Buffer::Usage usage, Buffer::MutableAccess access) noexcept {
            auto size = data.size() * sizeof(T);
            std::array<std::shared_ptr<Buffer>, 3> buffers;

            for (auto& buf : buffers) {
                buf = build(target, data, usage, access);
            }

            return std::make_unique<TripleBuffer>(std::move(buffers));
        }

        template<typename T>
        static std::unique_ptr<Buffer> buildTriple(Buffer::Type target, const std::vector<T>& data, Buffer::Storage flags, Buffer::ImmutableAccess access) {
            std::array<std::shared_ptr<Buffer>, 3> buffers;

            for (auto& buf : buffers) {
                auto immutable_storage = ContextInitializer::isExtensionSupported("GL_ARB_buffer_storage");

                if (immutable_storage) {
                    buf = build(target, data, flags, access);
                } else {
                    if (access == Buffer::ImmutableAccess::None) {
                        buf = build(target, data, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None);
                    } else {
                        buf = build(target, data, Buffer::Usage::StreamDraw, Buffer::MutableAccess::WriteUnsync);
                    }
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