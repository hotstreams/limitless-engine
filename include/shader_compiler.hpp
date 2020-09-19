#pragma once

#include <filesystem.hpp>

#include <shader.hpp>

#define SHADER_DIR "../shaders/"

namespace GraphicsEngine {
    class ShaderProgram;

    namespace {
        constexpr struct { std::string_view ext; ShaderType type; } shader_file_extensions[] = {
            { ".vs",  ShaderType::Vertex },
            { ".tcs", ShaderType::TessControl },
            { ".tes", ShaderType::TessEval },
            { ".gs",  ShaderType::Geometry },
            { ".fs",  ShaderType::Fragment },
            { ".cs",  ShaderType::Compute } };
    }

    class shader_linking_error : public std::runtime_error {
    public:
        explicit shader_linking_error(const std::string& error) : std::runtime_error(error) {}
    };

    class ShaderCompiler {
    protected:
        std::vector<Shader> shaders;
        static void checkStatus(GLuint program_id);
    public:
        ShaderCompiler() noexcept = default;
        ~ShaderCompiler() = default;

        ShaderCompiler(const ShaderCompiler&) noexcept = delete;
        ShaderCompiler& operator=(const ShaderCompiler&) noexcept = delete;

        ShaderCompiler(ShaderCompiler&&) noexcept = default;
        ShaderCompiler& operator=(ShaderCompiler&&) noexcept = default;

        std::shared_ptr<ShaderProgram> compile();

        using ShaderAction = std::function<void(Shader&)>;
        std::shared_ptr<ShaderProgram> compile(const fs::path& path, const ShaderAction& actions = ShaderAction{});

        ShaderCompiler& operator<<(Shader shader) noexcept;
    };
}