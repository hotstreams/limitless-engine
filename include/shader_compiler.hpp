#pragma once

#include <shader_program.hpp>
#include <shader_storage.hpp>
#include <functional>
#include <filesystem.hpp>

#define SHADER_DIR "../shaders/"

namespace GraphicsEngine {
    class ShaderCompiler {
    private:
        static std::string getFileSource(const fs::path& path);

        static void replaceKey(std::string& src, const std::string& key, const std::string& value) noexcept;
        static void replaceVersion(std::string& src) noexcept;
        static void replaceExtensions(std::string& src) noexcept;
        static void replaceSettings(std::string& src) noexcept;
        static void replaceIncludes(std::string& src);

        static GLuint createShader(GLuint shader_type, const GLchar* source);
        static GLuint createShaderProgram(const std::vector<GLuint>& id);

        static std::string getMaterialDefines(const MaterialType& type) noexcept;
        static std::string getModelDefines(const ModelShaderType& type) noexcept;
    public:
        using ShaderProperties = std::function<void(std::string&)>;
        [[nodiscard]] static std::shared_ptr<ShaderProgram> compile(const fs::path& path, const ShaderProperties& props_set = ShaderProperties{});
        static void compile(const MaterialType& type, uint64_t material_index, const RequiredMaterialShaders& material_types, const RequiredModelShaders& model_types);
    };
}