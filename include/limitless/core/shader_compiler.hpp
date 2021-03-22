#pragma once

#include <limitless/util/filesystem.hpp>
#include <functional>

#include <limitless/core/shader.hpp>

#define SHADER_DIR "../shaders/"

namespace LimitlessEngine {
    class ShaderProgram;

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

        ShaderCompiler& operator<<(Shader&& shader) noexcept;
    };
}