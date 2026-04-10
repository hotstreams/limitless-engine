#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

namespace Limitless {
    #ifdef LIMITLESS_OPENGL_DEBUG
        inline uint64_t gl_error_count = 0;

        void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

        void activate_debug();

        // Best-effort program name registry for debugging (works even without KHR_debug labels).
        void debug_register_program(GLuint program_id, const std::string& name);
        [[nodiscard]] std::string debug_get_program_name(GLuint program_id);
    #endif
}
