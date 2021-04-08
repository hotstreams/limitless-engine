#include <limitless/core/context_debug.hpp>
#include <iostream>
#include <glm/glm.hpp>

using namespace LimitlessEngine;

#ifdef GL_DEBUG

void LimitlessEngine::activate_debug() {
	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		std::cout << "OpenGL debug mode." << std::endl;

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
}

void LimitlessEngine::glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, [[maybe_unused]] GLsizei length, const GLchar* message, [[maybe_unused]] const void* userParam) {
	if (type == GL_DEBUG_TYPE_OTHER || severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " <<  message << std::endl;

	switch (source) {
		case GL_DEBUG_SOURCE_API:               std::cout << "Source: API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     std::cout << "Source: Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:   std::cout << "Source: Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:       std::cout << "Source: Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:       std::cout << "Source: Application"; break;
		case GL_DEBUG_SOURCE_OTHER:             std::cout << "Source: Other"; break;
	}
	std::cout << std::endl;

	switch (type) {
		case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; gl_error_count++; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	}
	std::cout << std::endl;

	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:            std::cout << "Severity: high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:          std::cout << "Severity: medium"; break;
		case GL_DEBUG_SEVERITY_LOW:             std::cout << "Severity: low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:    std::cout << "Severity: notification"; break;
	}
	std::cout << std::endl;
}

#endif
