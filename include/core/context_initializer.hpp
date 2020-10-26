#pragma once

#include <core/context_debug.hpp>

#include <string>
#include <vector>
#include <string_view>

namespace GraphicsEngine {
    struct ImplementationLimits {
        GLint uniform_buffer_max_count;
        GLint shader_storage_max_count;
        GLint max_texture_units;
    };

    class ContextInitializer {
    protected:
        static inline std::vector<std::string> extensions;
        static inline uint32_t context_count {0};

        static inline bool glfw_inited {false};
        static inline bool glew_inited {false};

        static void initializeGLEW();
        static void initializeGLFW();
        static void getExtensions() noexcept;
        static void getLimits() noexcept;

        ContextInitializer();
    public:
        static constexpr uint8_t major_version = 3;
        static constexpr uint8_t minor_version = 3;
        static inline ImplementationLimits limits;

        virtual ~ContextInitializer();

        static void defaultHints() noexcept;
        static void printExtensions() noexcept;
        static bool isExtensionSupported(std::string_view name) noexcept;
    };
}