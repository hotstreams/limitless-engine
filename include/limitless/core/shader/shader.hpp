#pragma once

#include <limitless/core/context_debug.hpp>

#include <limitless/util/filesystem.hpp>
#include <fstream>
#include <utility>

namespace Limitless {
    class shader_file_not_found : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class shader_include_not_found : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class shader_compilation_error : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    /**
     * Shader class describes shader that is used to compile shader program by ShaderCompiler
     *
     * ShaderProgram consists of several different shader types that must be compiled together
     */
    class Shader {
    public:
        /**
         * Defines shader types that can be used in one shader program
         */
        enum class Type {
            Vertex = GL_VERTEX_SHADER,
            TessEval = GL_TESS_EVALUATION_SHADER,
            TessControl = GL_TESS_CONTROL_SHADER,
            Geometry = GL_GEOMETRY_SHADER,
            Fragment = GL_FRAGMENT_SHADER,
            Compute = GL_COMPUTE_SHADER
        };
    private:
        /**
         * Shader source code loaded from file
         */
        std::string source;

        /**
         * Filesystem path of this shader
         */
        fs::path path;

        /**
         * Shader type
         */
        Type type {Type::Vertex};

        /**
         * Unique object identifier
         */
        GLuint id {};

        /**
         * Replaces key-line "Limitless::Extensions" in shader source code with extensions enabled by Engine
         *
         * Replaced extensions depend on functionality used by Engine and GPU capabilities
         */
        void replaceExtensions() noexcept;

        /**
         * Replaces key-line "Limitless::GLSL_VERSION" in shader source code with version used by Engine
         *
         * It is always 3.3 version for now (Advanced functionality supported by use of extensions)
         */
        void replaceVersion() noexcept;

        /**
         * Self-implemented "#include" directive for GLSL
         *
         * Resolves recursive includes in shader source code relative to @param base_dir
         *
         * @param base_dir - directory file path at which current include file is located
         */
        void replaceIncludes(const fs::path& base_dir);
        static void resolveIncludes(const fs::path& base_dir, std::string& src);

        /**
         * Checks compilation status of shader
         */
        void checkStatus() const;

        Shader() = default;

        /**
         * Reads source code of file specified at @param filepath
         *
         * @param filepath - filepath to read a file from
         * @return - file source code
         * @throw shader_file_not_found - if file at @param filepath does not exist
         */
        static std::string getSource(const fs::path& filepath);

        /**
         * Swap function used by move-semantics
         */
        friend void swap(Shader& lhs, Shader&rhs) noexcept;
    public:
        /**
         * ShaderAction is custom action that should be applied to source code before its compilation
         *
         * It is used by some ShaderCompilers (like material and effect) to replace some property-dependent key-lines in source code using "replaceKey"
         */
        using ShaderAction = std::function<void(Shader&)>;

        /**
         * Shader constructor
         *
         * Tries to load shader source code from file at @param path, creates object with specified @param type,
         * replaces engine key-lines and applies @param action to source code
         *
         * @param path
         * @param type
         * @param action
         *
         * @throw shader_file_not_found - if file at @param path does not exist
         * @throw shader_include_not_found - if file specified at include directive does not exist
         */
        Shader(fs::path path, Type type, const ShaderAction& action = {});

        /**
         * Shader destructor
         *
         * Clean ups shader object id from OpenGL context
         */
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        Shader(Shader&&) noexcept;
        Shader& operator=(Shader&&) noexcept;

        [[nodiscard]] const auto& getId() const noexcept { return id; }

        /**
         * Compiles shader
         *
         * @throw shader_compilation_error - if there are some errors encountered during compilation
         *
         * Creates shader_compilation_error file with dumped error and source code
         */
        void compile() const;

        /**
         * Replaces specified key-line in shader source code with specified value
         *
         * @param key - key-line to be replaced in shader source code
         * @param value - replaced with
         */
        void replaceKey(const std::string& key, const std::string& value) noexcept;

        const auto& getSource() const noexcept { return source; }
    };

    void swap(Shader& lhs, Shader&rhs) noexcept;
}
