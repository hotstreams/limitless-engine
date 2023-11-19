#include <limitless/core/shader/shader_compiler.hpp>

#include <fstream>
#include <limitless/core/context.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include <limitless/core/shader/shader_extensions.hpp>
#include <limitless/renderer/render_settings_shader_definer.hpp>
#include <limitless/core/shader/shader_defines.hpp>
#include <limitless/core/keyline_extensions.hpp>

using namespace Limitless;

ShaderCompiler::ShaderCompiler(Context& _context)
    : context {_context} {
}

ShaderCompiler::ShaderCompiler(Context& _context, const RenderSettings& _settings)
    : context {_context}
    , render_settings {_settings} {
}

void ShaderCompiler::checkStatus(const GLuint program_id) {
    GLint link_status;
    glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);

    if (!link_status) {
        GLint log_size = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_size);

        std::string log;
        log.resize(log_size);
        glGetProgramInfoLog(program_id, log_size, &log_size, log.data());

        glDeleteProgram(program_id);

        {
            std::ofstream file("shader_linking_error");
            file << log << std::endl;
        }

        throw shader_linking_error(log);
    }
}

ShaderCompiler& ShaderCompiler::operator<<(Shader&& shader) noexcept {
    shaders.emplace_back(std::move(shader));

    return *this;
}

std::shared_ptr<ShaderProgram> ShaderCompiler::compile() {
    if (shaders.empty()) {
        throw shader_linking_error("No shaders to link. ShaderCompiler is empty.");
    }

    const GLuint program_id = glCreateProgram();

    for (const auto& shader : shaders) {
        shader.compile();

        glAttachShader(program_id, shader.getId());
    }

    glLinkProgram(program_id);

    checkStatus(program_id);

    shaders.clear();

    return std::shared_ptr<ShaderProgram>(new ShaderProgram(program_id));
}

void ShaderCompiler::replaceEngineDefines(Shader& shader) const {
    shader.replaceKey(ENGINE_SHADER_DEFINE_NAMES[EngineShaderDefine::GLSLVersion], getVersionDefine());
    shader.replaceKey(ENGINE_SHADER_DEFINE_NAMES[EngineShaderDefine::Extensions], getExtensionsDefine());
    shader.replaceKey(ENGINE_SHADER_DEFINE_NAMES[EngineShaderDefine::Settings], getSettingsDefine());
    shader.replaceKey(ENGINE_SHADER_DEFINE_NAMES[EngineShaderDefine::Common], getCommonDefine());
}

std::shared_ptr<ShaderProgram> ShaderCompiler::compile(const fs::path& path, const ShaderAction& action) {
    uint8_t shader_count {};
    for (const auto& [extension, type] : shader_file_extensions) {
        try {
            Shader shader { path.string() + extension.data(), type, action };

            replaceEngineDefines(shader);

            std::string e = type == Shader::Type::Vertex ? ".vert" : ".frag";
            // TODO: temp ref/remove
            static int i = 0;
            std::ofstream f {"D:/Dev/Projects/limitless-engine/glslang/" + std::to_string(i++) + e};
            f << shader.getSource();
            f.close();

            *this << std::move(shader);

            ++shader_count;
        } catch (const shader_file_not_found& e) {
            continue;
        }
    }

    if (shader_count == 0) {
        throw shader_compilation_error("Shaders not found : " + path.string());
    }

    return compile();
}

std::string ShaderCompiler::getVersionDefine() {
    return "#version " + std::to_string(ContextInitializer::major_version) + std::to_string(ContextInitializer::minor_version) + "0 core\n";
}

std::string ShaderCompiler::getExtensionsDefine() {
    std::string extensions;

    if (ContextInitializer::isExtensionSupported(shader_storage_buffer_object)) {
        extensions.append(extension_shader_storage_buffer_object);
    }

    if (ContextInitializer::isExtensionSupported(shading_language_420pack)) {
        extensions.append(extension_shading_language_420pack);
    }

    if (ContextInitializer::isExtensionSupported(explicit_uniform_location)) {
        extensions.append(extension_explicit_uniform_location);
    }

    if (ContextInitializer::isExtensionSupported(bindless_texture)) {
        extensions.append(extension_bindless_texture);
        extensions.append(bindless_texture_define);
        extensions.append(bindless_samplers);
    }

    return extensions;
}

std::string ShaderCompiler::getSettingsDefine() const {
    std::string define;
    if (render_settings) {
        RenderSettingsShaderDefiner settings {*render_settings};
        define = settings.define();
    }
    return define;
}

std::string ShaderCompiler::getCommonDefine() const {
    std::string defines = getVersionDefine();
    defines.append(getExtensionsDefine());
    defines.append(getSettingsDefine());
    return defines;
}
