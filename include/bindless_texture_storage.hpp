#pragma once

#include <buffer_builder.hpp>

namespace GraphicsEngine {
    struct Handle {
        GLuint64 handle {0};
        bool alive {false};
    };

    class BindlessTextureStorage {
    private:
        static constexpr size_t max_handles = 128;
        static inline std::array<Handle, max_handles> handles;

        static inline std::shared_ptr<Buffer> handle_storage;
        static inline bool inited {false};
    public:
        static void add(GLuint64 new_handle);
        static void remove(GLuint64 old_handle) noexcept;

        static void map();
        [[nodiscard]] static uint64_t getIndex(GLuint64 handle);

        static void init();
    };
}