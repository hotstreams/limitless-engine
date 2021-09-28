#pragma once

#include <limitless/util/filesystem.hpp>
#include <functional>

#include <limitless/core/shader.hpp>
#include <limitless/pipeline/render_settings.hpp>

namespace Limitless {
    class ShaderProgram;
    class RenderSettings;
    class Context;

    class shader_linking_error : public std::runtime_error {
    public:
        explicit shader_linking_error(const std::string& error) : std::runtime_error(error) {}
    };

    class ShaderCompiler {
    protected:
        std::vector<Shader> shaders;
        static void checkStatus(GLuint program_id);
        Context& context;

        // optional RenderSettings to replace Limitless::Settings
        std::optional<RenderSettings> render_settings;

        void replaceRenderSettings(Shader& shader) const;
    public:
        ShaderCompiler(Context& ctx, const RenderSettings& settings);
        explicit ShaderCompiler(Context& ctx);
        virtual ~ShaderCompiler() = default;

        ShaderCompiler(const ShaderCompiler&) noexcept = delete;
        ShaderCompiler& operator=(const ShaderCompiler&) noexcept = delete;

        std::shared_ptr<ShaderProgram> compile();

        using ShaderAction = std::function<void(Shader&)>;
        std::shared_ptr<ShaderProgram> compile(const fs::path& path, const ShaderAction& actions = ShaderAction{});

        ShaderCompiler& operator<<(Shader&& shader) noexcept;
    };
}