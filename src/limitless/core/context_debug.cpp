#include <limitless/core/context_debug.hpp>
#include <iostream>
#include <array>
#include <string_view>
#include <unordered_map>
#include <mutex>
#include <glm/glm.hpp>

using namespace Limitless;

#ifdef LIMITLESS_OPENGL_DEBUG

namespace {
	std::mutex g_programNamesMutex;
	std::unordered_map<GLuint, std::string> g_programNames;

	[[nodiscard]] static std::string getObjectLabelSafe(GLenum identifier, GLuint name) {
		// Requires KHR_debug / GL 4.3. If not available, return empty.
		if (!(GLEW_KHR_debug || GLEW_VERSION_4_3)) {
			return {};
		}

		// Some drivers don't report length correctly when bufSize==0.
		// Use a small fixed buffer and read the label in one go.
		std::array<char, 256> buf{};
		GLsizei written = 0;
		glGetObjectLabel(identifier, name, static_cast<GLsizei>(buf.size()), &written, buf.data());
		if (written <= 0) return {};
		return std::string(buf.data(), buf.data() + written);
	}

	static void dumpShadowSamplerDebug() {
		// Print current program and its label (if any).
		GLint program = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &program);
		std::cout << "Active program: " << program;
		if (program) {
			const auto label = getObjectLabelSafe(GL_PROGRAM, static_cast<GLuint>(program));
			if (!label.empty()) {
				std::cout << " (label: " << label << ")";
			}
			{
				std::scoped_lock lk(g_programNamesMutex);
				auto it = g_programNames.find(static_cast<GLuint>(program));
				if (it != g_programNames.end() && !it->second.empty()) {
					std::cout << " (name: " << it->second << ")";
				} else {
					std::cout << " (name: <unregistered>, registry size=" << g_programNames.size() << ")";
					// Print a few known entries to validate registry is populated.
					size_t shown = 0;
					for (const auto& [pid, pname] : g_programNames) {
						if (shown++ >= 5) break;
						std::cout << "\n  registry[" << pid << "] = " << pname;
					}
				}
			}
		}
		std::cout << std::endl;

		// Inspect the shadow sampler uniform if present: which unit is it set to?
		if (program) {
			const GLint loc = glGetUniformLocation(static_cast<GLuint>(program), "_dir_shadows");
			if (loc >= 0) {
				GLint unit = -1;
				glGetUniformiv(static_cast<GLuint>(program), loc, &unit);
				std::cout << "_dir_shadows uniform unit: " << unit << std::endl;

				// Query bindings for that unit.
				GLint prevActive = 0;
				glGetIntegerv(GL_ACTIVE_TEXTURE, &prevActive);
				if (unit >= 0) {
					glActiveTexture(GL_TEXTURE0 + unit);
					GLint tex2D = 0;
					GLint tex2DArray = 0;
					glGetIntegerv(GL_TEXTURE_BINDING_2D, &tex2D);
					glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &tex2DArray);
					std::cout << "Bindings for unit " << unit << ": 2D=" << tex2D
							  << " 2D_ARRAY=" << tex2DArray << std::endl;

					if (tex2DArray) {
						GLint cmpMode = 0, cmpFunc = 0;
						glBindTexture(GL_TEXTURE_2D_ARRAY, tex2DArray);
						glGetTexParameteriv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, &cmpMode);
						glGetTexParameteriv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, &cmpFunc);
						std::cout << "unit " << unit << " 2D_ARRAY compareMode=" << cmpMode
								  << " compareFunc=" << cmpFunc << std::endl;
					}
				}
				glActiveTexture(prevActive);
			} else {
				std::cout << "_dir_shadows uniform not found in active program." << std::endl;
			}
		}
	}
}

void Limitless::debug_register_program(GLuint program_id, const std::string& name) {
	if (!program_id || name.empty()) return;
	std::scoped_lock lk(g_programNamesMutex);
	g_programNames[program_id] = name;
}

std::string Limitless::debug_get_program_name(GLuint program_id) {
	std::scoped_lock lk(g_programNamesMutex);
	auto it = g_programNames.find(program_id);
	return it != g_programNames.end() ? it->second : std::string{};
}

void Limitless::activate_debug() {
	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		std::cerr << "OpenGL debug mode" << std::endl;

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
}

void Limitless::glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, [[maybe_unused]] GLsizei length, const GLchar* message, [[maybe_unused]] const void* userParam) {
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

	// Extra diagnostics for common "shadow sampler bound to texture 0" UB warning.
	// This helps pinpoint which program is sampling a shadow sampler without proper bindings.
	std::string_view msg{message ? message : ""};
	if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR &&
		(id == 131222u || msg.find("shadow sampler") != std::string_view::npos)) {
		dumpShadowSamplerDebug();
	}
}

#endif
