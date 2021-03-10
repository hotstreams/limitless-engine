#pragma once

#include <core/context_debug.hpp>

#include <util/filesystem.hpp>
#include <fstream>
#include <utility>

#define SHADER_DIR "../shaders/"

namespace LimitlessEngine {
    class shader_file_not_found : public std::runtime_error {
    public:
        explicit shader_file_not_found(const std::string& error) : std::runtime_error(error) {}
    };

    class shader_compilation_error : public std::runtime_error {
    public:
        explicit shader_compilation_error(const std::string& error) : std::runtime_error(error) {}
    };

    class Shader {
    public:
        enum class Type {
            Vertex = GL_VERTEX_SHADER,
            TessEval = GL_TESS_EVALUATION_SHADER,
            TessControl = GL_TESS_CONTROL_SHADER,
            Geometry = GL_GEOMETRY_SHADER,
            Fragment = GL_FRAGMENT_SHADER,
            Compute = GL_COMPUTE_SHADER

            // easter egg bonus; TODO: include "GL/glext.h"
            // Task = GL_TASK_SHADER_NV,
            // Mesh = GL_MESH_SHADER_NV
        };
    private:
        std::string source;
        fs::path path;
        Type type {Type::Vertex};
        GLuint id {0};

        void replaceExtensions() noexcept;
        void replaceVersion() noexcept;
        void replaceIncludes();

        void checkStatus() const;

        Shader() = default;
        static std::string getSource(const fs::path &filepath);
        friend void swap(Shader& lhs, Shader&rhs) noexcept;
    public:
        Shader(fs::path path, Type type);
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        Shader(Shader&&) noexcept;
        Shader& operator=(Shader&&) noexcept;

        [[nodiscard]] const auto& getId() const noexcept { return id; }

        void compile() const;

        void replaceKey(const std::string& key, const std::string& value) noexcept;
    };

    void swap(Shader& lhs, Shader&rhs) noexcept;
}