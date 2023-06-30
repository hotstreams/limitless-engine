#pragma once

#include <limitless/util/filesystem.hpp>
#include <functional>
#include <optional>

#include <limitless/core/shader/shader.hpp>
#include <limitless/pipeline/render_settings.hpp>

namespace Limitless {
    class ShaderProgram;
    class RenderSettings;
    class Context;

    class shader_linking_error : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    /**
     * ShaderCompiler compiles Shaders and assembles ShaderProgram
     *
     * Use compile function to compile ShaderProgram at specified path with name
     */
    class ShaderCompiler {
    protected:
        /**
         * Current ShaderProgram shaders
         */
        std::vector<Shader> shaders;

        /**
         * Checks compilation status of ShaderProgram
         *
         * @param program_id - ShaderProgram object id
         */
        static void checkStatus(GLuint program_id);

        /**
         * Context
         */
        Context& context;

        /**
         * RenderSettings to compile shader with
         */
        std::optional<RenderSettings> render_settings;

        /**
         * Sets up ShaderAction to be applied to shader source code
         *
         * Replaces key-line with used settings
         *
         * @param shader - shader to be applied to
         */
        void replaceRenderSettings(Shader& shader) const;
    public:
        /**
         * Shader constructor
         *
         * Context is used to initialize internal structures of ShaderProgram
         *
         * ShaderCompiler gets properties from RenderSettings to set up quality and enable/disable functionality
         *
         * @param ctx
         * @param settings
         */
        ShaderCompiler(Context& ctx, const RenderSettings& settings);
        explicit ShaderCompiler(Context& ctx);
        virtual ~ShaderCompiler() = default;

        ShaderCompiler(const ShaderCompiler&) noexcept = delete;
        ShaderCompiler& operator=(const ShaderCompiler&) noexcept = delete;

        using ShaderAction = std::function<void(Shader&)>;
        /**
         * Compiles ShaderProgram at specified path and applies ShaderActions
         *
         * Takes name from path and tries to compile name.ext files as shaders and combine them in ShaderProgram
         *
         * check shader file extensions in "shader_extensions.hpp"
         *
         * Example:
         *          /home/user/
         *                 shader_name.vs
         *                 shader_name.fs
         * ShaderCompiles will compile shader_name ShaderProgram with shader_name.vs as VertexShader and shader_name.fs as FragmentShader
         *
         * @param path - path with name of ShaderProgram
         * @param actions - ShaderActions to be applied to source code
         * @return - compiled ShaderProgram
         */
        std::shared_ptr<ShaderProgram> compile(const fs::path& path, const ShaderAction& actions = ShaderAction{});


        /**
         * Adds Shader to ShaderProgram and waits to compile function to be invoked on
         *
         * So you can manually compile ShaderProgram with self-loaded Shaders
         *
         * @param shader - shader to be added to ShaderProgram
         * @return self
         */
        ShaderCompiler& operator<<(Shader&& shader) noexcept;

        /**
         * Compiles ShaderProgram with stored Shaders
         * @return
         */
        std::shared_ptr<ShaderProgram> compile();
    };
}