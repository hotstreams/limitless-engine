#include <limitless/core/shader_compiler.hpp>

#include <fstream>
#include <limitless/core/context.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/pipeline/render_settings.hpp>

using namespace Limitless;

namespace {
    constexpr struct { std::string_view ext; Shader::Type type; } shader_file_extensions[] = {
        { ".vs",  Shader::Type::Vertex },
        { ".tcs", Shader::Type::TessControl },
        { ".tes", Shader::Type::TessEval },
        { ".gs",  Shader::Type::Geometry },
        { ".fs",  Shader::Type::Fragment },
        { ".cs",  Shader::Type::Compute }
    };
}

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

    return std::shared_ptr<ShaderProgram>(new ShaderProgram(context, program_id));
}

void ShaderCompiler::replaceRenderSettings(Shader& shader) const {
    if (render_settings) {
        std::string settings;

        //TODO: remove
        if (render_settings->normal_mapping) {
            settings.append("#define NORMAL_MAPPING\n");
        }

        if (render_settings->directional_cascade_shadow_mapping) {
            settings.append("#define DIRECTIONAL_CSM\n");

            settings.append("#define DIRECTIONAL_SPLIT_COUNT " + std::to_string(render_settings->directional_split_count) + '\n');

            if (render_settings->directional_pcf) {
                settings.append("#define DIRECTIONAL_PFC\n");
            }
        }

        if (render_settings->screen_space_ambient_occlusion) {
	        settings.append("#define SCREEN_SPACE_AMBIENT_OCCLUSION\n");
        }

        if (render_settings->micro_shadowing) {
            settings.append("#define MICRO_SHADOWING\n");
        }

        shader.replaceKey("Limitless::Settings", settings);
    } else {
        shader.replaceKey("Limitless::Settings", "");
    }
}

std::shared_ptr<ShaderProgram> ShaderCompiler::compile(const fs::path& path, const ShaderAction& action) {
    uint8_t shader_count {};
    for (const auto& [extension, type] : shader_file_extensions) {
        try {
            Shader shader { path.string() + extension.data(), type, action };

            replaceRenderSettings(shader);

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
