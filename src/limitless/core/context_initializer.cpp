#include <limitless/core/context_initializer.hpp>

#include <algorithm>
#include <fstream>

using namespace Limitless;

void ContextInitializer::initializeGLEW() {
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW.");
    }

#ifdef GL_DEBUG
    activate_debug();
#endif

    getExtensions();
    getLimits();

#ifdef GL_DEBUG
    printExtensions();
#endif

    glew_inited = true;
}

void ContextInitializer::initializeGLFW() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_version);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef GL_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    glfw_inited = true;
}

ContextInitializer::ContextInitializer() {
    if (!glfw_inited) {
        initializeGLFW();
    }

    ++context_count;
}

ContextInitializer::~ContextInitializer() {
    if (--context_count == 0) {
        glfwTerminate();
        glfw_inited = false;
        glew_inited = false;
    }
}

void ContextInitializer::getExtensions() noexcept {
    GLint count;
    glGetIntegerv(GL_NUM_EXTENSIONS, &count);

    extensions.reserve(count);
    for (GLint i = 0; i < count; ++i) {
        const auto name = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
        extensions.emplace_back(name);
    }

	#ifdef OPENGL_TOSTER
		extensions.clear();
        extensions.emplace_back("GL_ARB_shader_storage_buffer_object");
        extensions.emplace_back("GL_ARB_shading_language_420pack");
        extensions.emplace_back("GL_ARB_explicit_uniform_location");
	#endif
}

bool ContextInitializer::isExtensionSupported(std::string_view name) noexcept {
    return std::find(extensions.cbegin(), extensions.cend(), name) != extensions.cend();
}

void ContextInitializer::getLimits() noexcept {
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &limits.uniform_buffer_max_count);
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &limits.shader_storage_max_count);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &limits.max_texture_units);

    if (isExtensionSupported("GL_EXT_texture_filter_anisotropic")) {
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &limits.anisotropic_max);
    }
}

void ContextInitializer::printExtensions() noexcept {
    std::ofstream file("extensions.txt");

    for (const auto& extension : extensions) {
        file << extension << std::endl;
    }
}

void ContextInitializer::defaultHints() noexcept {
    glfwDefaultWindowHints();
}

bool ContextInitializer::checkMinimumRequirements() noexcept {
    std::vector<std::string_view> requirements = {
        "GL_ARB_shader_storage_buffer_object",
        "GL_ARB_shading_language_420pack",
        "GL_ARB_explicit_uniform_location",
    };

    return std::all_of(requirements.begin(), requirements.end(), [] (const auto& extension) {
        return isExtensionSupported(extension);
    });
}
