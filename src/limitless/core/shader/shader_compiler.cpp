#include <limitless/core/shader/shader_compiler.hpp>

#include <fstream>
#include <limitless/core/context.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include <limitless/core/shader/shader_extensions.hpp>
#include <limitless/renderer/render_settings_shader_definer.hpp>
#include <limitless/core/shader/shader_define_replacer.hpp>
#include <limitless/core/keyline_extensions.hpp>

using namespace Limitless;

ShaderCompiler::ShaderCompiler(Context& _context)
    : context {_context} {
}

ShaderCompiler::ShaderCompiler(Context& _context, const RendererSettings& _settings)
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

void ShaderCompiler::replaceCommonDefines(Shader &shader) {
    ShaderDefineReplacer::replaceCommon(shader, render_settings);
}

std::shared_ptr<ShaderProgram> ShaderCompiler::compile(const fs::path& path, const ShaderAction& action) {
    uint8_t shader_count {};
    for (const auto& [extension, type] : shader_file_extensions) {
        try {
            Shader shader { path.string() + extension.data(), type, action };

            replaceCommonDefines(shader);

            *this << std::move(shader);

            ++shader_count;
        } catch (const shader_file_not_found& e) {
            continue;
        }
    }

    if (shader_count == 0) {
        throw shader_compilation_error("Shaders not found: " + path.string());
    }

    return compile();
}

