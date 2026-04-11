#pragma once

#include <limitless/core/context_debug.hpp>

#include <string>
#include <vector>
#include <string_view>

namespace Limitless {
    struct ImplementationLimits {
        GLint uniform_buffer_max_count;
        GLint shader_storage_max_count;
        GLint max_texture_units;
        GLint max_tess_level;

        GLfloat anisotropic_max {0.0f};
    };

    class ContextInitializer {
    protected:
        static inline std::vector<std::string> extensions;
        static inline GLint gl_major_version {};
        static inline GLint gl_minor_version {};
        static inline uint32_t context_count {};

        static inline bool glfw_inited {};
        static inline bool glew_inited {};

        static void initializeGLEW();
        static void initializeGLFW();
        static void discoverExtensions() noexcept;
        static void getLimits() noexcept;

        ContextInitializer();
    public:
        static constexpr uint8_t major_version = 4;
        static constexpr uint8_t minor_version = 6;
        static inline ImplementationLimits limits;

        virtual ~ContextInitializer();

        static void defaultHints() noexcept;
        static void printExtensions() noexcept;
        static bool isExtensionSupported(std::string_view name) noexcept;
        static bool checkMinimumRequirements() noexcept;
        static const auto& getExtensions() noexcept {
            return extensions;
        }

        static bool isProgramInterfaceQuerySupported() noexcept;
        static bool isBindlessTextureSupported() noexcept;
        static bool isImmutableTextureSupported() noexcept;
        static bool isNamedTextureSupported() noexcept;

        /** OpenGL context major/minor (from GL_MAJOR_VERSION / GL_MINOR_VERSION after context creation). */
        [[nodiscard]] static GLint getGlMajorVersion() noexcept { return gl_major_version; }
        [[nodiscard]] static GLint getGlMinorVersion() noexcept { return gl_minor_version; }

        /** GL_ARB_base_instance or OpenGL 4.2+ */
        [[nodiscard]] static bool supportsBaseInstance() noexcept;

        /** GL_ARB_shader_draw_parameters or OpenGL 4.3+ (gl_BaseInstance in GLSL) */
        [[nodiscard]] static bool supportsShaderDrawParametersGlsl() noexcept;

        /**
         * GLSL version implied by major_version/minor_version (e.g. 4.6 -> 460).
         */
        [[nodiscard]] static int getRequestedGlslVersion() noexcept;

        /**
         * True if coarse derivatives (dFdxCoarse) are in GLSL core without extension.
         */
        [[nodiscard]] static bool glslCoarseDerivativesBuiltin() noexcept;

        /**
         * Emit #extension GL_ARB_derivative_control when GLSL < 4.5 but the driver exposes the extension.
         */
        [[nodiscard]] static bool shouldEnableDerivativeControlExtension() noexcept;

        /**
         * Fallback #define lines for util.glsl helpers (must appear after all #extension in the preamble).
         */
        [[nodiscard]] static std::string getGlslBuiltinFallbackDefines() noexcept;
    };
}