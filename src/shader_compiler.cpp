#include <shader_compiler.hpp>

#include <fstream>
#include <context.hpp>
#include <iostream>

using namespace GraphicsEngine;

std::string ShaderCompiler::getFileSource(const fs::path& path) {
    std::ifstream file;
    std::string s;

    file.open(path);
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    if (file.good()) {
        std::stringstream stream;
        stream << file.rdbuf();

        file.close();
        s = stream.str();
    }

    return s;
}

void ShaderCompiler::replaceVersion(std::string& src) noexcept {
    static std::string glsl_version = "#version " + std::to_string(ContextInitializer::major_version)
                                                  + std::to_string(ContextInitializer::minor_version)
                                                  + "0 core";

    replaceKey(src, "GraphicsEngine::GLSL_VERSION", glsl_version);
}

void ShaderCompiler::replaceExtensions(std::string &src) noexcept {
    std::string extensions;

    if (ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
        extensions.append("#extension GL_ARB_bindless_texture : require\n");
        extensions.append("#define BINDLESS_TEXTURE\n");
    }

    replaceKey(src, "GraphicsEngine::Extensions", extensions);
}

void ShaderCompiler::replaceKey(std::string &src, const std::string& key, const std::string& value) noexcept {
    size_t found = 0;
    for (;;) {
        found = src.find(key, found);

        if (found == std::string::npos) {
            break;
        }

        src.replace(found, key.length(), value);
    }
}

void ShaderCompiler::replaceIncludes(std::string& src) {
    static constexpr std::string_view include = "#include";

    size_t found = 0;
    for (;;) {
        found = src.find(include, found);

        if (found == std::string::npos) {
            break;
        }

        size_t beg = found + include.length() + 2;
        size_t end = src.find('"', beg);
        size_t name_length = end - beg;

        std::string file_name = src.substr(beg, name_length);

        try {
            const auto& include_src = getFileSource(SHADER_DIR + file_name);
            src.replace(found, include.length() + 3 + name_length, include_src);
        } catch(const std::exception& e) {
            throw std::runtime_error("Cannot open shader's include file " + file_name);
        }
    }
}

GLuint ShaderCompiler::createShader(GLuint shader_type, const GLchar* source) {
    GLuint id = glCreateShader(shader_type);

    glShaderSource(id, 1, &source, nullptr);
    glCompileShader(id);

    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLint log_size = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_size);

        std::string log;
        log.resize(log_size);
        glGetShaderInfoLog(id, log_size, &log_size, log.data());

        glDeleteShader(id);

        throw std::runtime_error("Failed to compile shader: " + log);
    }

    return id;
}

GLuint ShaderCompiler::createShaderProgram(const std::vector<GLuint>& id) {
    GLuint program_id = glCreateProgram();

    for (const auto& shader_id : id) {
        glAttachShader(program_id, shader_id);
    }

    glLinkProgram(program_id);

    GLint link_status;
    glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);

    if (!link_status) {
        GLint log_size = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_size);

        std::string log;
        log.resize(log_size);
        glGetProgramInfoLog(program_id, log_size, &log_size, log.data());

        glDeleteProgram(program_id);

        throw std::runtime_error("Failed to link program: " + log);
    }

    return program_id;
}

std::shared_ptr<ShaderProgram> ShaderCompiler::compile(const fs::path& path, const ShaderProperties& props_set) {
    static constexpr std::pair<std::string_view, GLuint> shaders[] = {
            { ".vs", GL_VERTEX_SHADER },
            { ".fs", GL_FRAGMENT_SHADER },

            { ".gs", GL_GEOMETRY_SHADER },
            { ".tcs", GL_TESS_CONTROL_SHADER },
            { ".tes", GL_TESS_EVALUATION_SHADER },

//            { ".ts", GL_TASK_SHADER_NV },
//            { ".ms", GL_MESH_SHADER_NV }
    };

    std::vector<GLuint> shader_id;

    for (const auto& [ext, type] : shaders) {
        try {
            std::string src = getFileSource(path.string() + ext.data());

            replaceVersion(src);
            replaceIncludes(src);
            replaceExtensions(src);

            if (props_set) {
                props_set(src);
            }

            auto id = createShader(type, src.c_str());
            shader_id.emplace_back(id);
        } catch (const std::ifstream::failure& e) {
            continue;
        }
    }

    if (shader_id.empty()) {
        throw std::runtime_error("Failed to find any shader file named " + path.string());
    }

    auto id = createShaderProgram(shader_id);

    return std::shared_ptr<ShaderProgram>(new ShaderProgram(id));
}

std::string ShaderCompiler::getMaterialDefines(const MaterialType &type) noexcept {
    std::string property_defines;
    for (const auto& property : type.properties) {
        switch (property) {
            case PropertyType::Color:
                property_defines.append("#define MATERIAL_COLOR\n");
                break;
            case PropertyType::EmissiveColor:
                property_defines.append("#define MATERIAL_EMISSIVE_COLOR\n");
                break;
            case PropertyType::Diffuse:
                property_defines.append("#define MATERIAL_DIFFUSE\n");
                break;
            case PropertyType::Specular:
                property_defines.append("#define MATERIAL_SPECULAR\n");
                break;
            case PropertyType::Normal:
                property_defines.append("#define MATERIAL_NORMAL\n");
                break;
            case PropertyType::Displacement:
                property_defines.append("#define MATERIAL_DISPLACEMENT\n");
                break;
            case PropertyType::EmissiveMask:
                property_defines.append("#define MATERIAL_EMISSIVEMASK\n");
                break;
            case PropertyType::BlendMask:
                property_defines.append("#define MATERIAL_BLENDMASK\n");
                break;
            case PropertyType::Shininess:
                property_defines.append("#define MATERIAL_SHININESS\n");
                break;
            case PropertyType::Metallic:
                property_defines.append("#define MATERIAL_METALLIC\n");
                break;
            case PropertyType::Roughness:
                property_defines.append("#define MATERIAL_ROUGHNESS\n");
                break;
        }
    }

    switch (type.shading) {
        case Shading::Lit:
            property_defines.append("#define MATERIAL_LIT\n");
            break;
        case Shading::Unlit:
            property_defines.append("#define MATERIAL_UNLIT\n");
            break;
    }

    return property_defines;
}

void ShaderCompiler::compile(const MaterialType& type, uint64_t material_index, const RequiredShaders& shaders) {
    auto material_props = [&] (std::string& src) {
        replaceKey(src, "GraphicsEngine::MaterialType", getMaterialDefines(type));
    };

    for (const auto& required : shaders) {
        shader_storage.add(required, material_index, compile(SHADER_DIR + material_shader_path.at(required), material_props));
    }
}