#pragma once

#include <context_debug.hpp>

#include <vector>
#include <string_view>

namespace GraphicsEngine {
    struct ImplementationLimits {
        GLint uniform_buffer_max_count;
        GLint shader_storage_max_count;
        GLint max_texture_units;
    };

    enum class WindowHint {
        Resizable = GLFW_RESIZABLE,
        Visible = GLFW_VISIBLE,
        Decorated = GLFW_DECORATED,
        Focused = GLFW_FOCUSED,
        AutoIconify = GLFW_AUTO_ICONIFY,
        Maximized = GLFW_MAXIMIZED,
        Samples = GLFW_SAMPLES
    };
    using WindowHints = std::vector<std::pair<WindowHint, int>>;

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
    public:
        static constexpr uint8_t major_version = 3;
        static constexpr uint8_t minor_version = 3;
        static inline ImplementationLimits limits;

        ContextInitializer();
        virtual ~ContextInitializer();

        static void defaultHints() noexcept;
        static void printExtensions() noexcept;
        static bool isExtensionSupported(std::string_view name) noexcept;
    };
}