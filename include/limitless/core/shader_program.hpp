#pragma once

#include <limitless/core/indexed_buffer.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace LimitlessEngine {
    template<typename T> class UniformValue;
    class UniformSampler;
    class Material;
    class Uniform;
    class ContextState;

    struct shader_program_error : public std::runtime_error {
        explicit shader_program_error(const char* error) noexcept : runtime_error{error} {}
    };

    class ShaderProgram final {
    private:
        GLuint id{};
        // stores uniform locations inside the shader
        std::unordered_map<std::string, GLint> locations;
        // stores indexed buffers binding data
        std::vector<IndexedBufferData> indexed_binds;
        // stores uniform values
        std::map<std::string, std::unique_ptr<Uniform>> uniforms;

        GLint getUniformLocation(const Uniform& uniform) const noexcept;

        void getUniformLocations() noexcept;
        void getIndexedBufferBounds(ContextState& ctx) noexcept;

        void bindIndexedBuffers(ContextState& ctx);
        void bindTextures() const noexcept;

        ShaderProgram() noexcept = default;
        ShaderProgram(ContextState& ctx, GLuint id);

        template<typename T> friend class UniformValue;
        friend class UniformSampler;
        friend class MaterialCompiler;
        friend class ShaderCompiler;
        friend class MaterialBuilder;
        friend void swap(ShaderProgram& lhs, ShaderProgram& rhs) noexcept;
    public:
        ~ShaderProgram();

        void use();

        [[nodiscard]] auto getId() const noexcept { return id; }

        ShaderProgram(const ShaderProgram&) noexcept = delete;
        ShaderProgram& operator=(const ShaderProgram&) noexcept = delete;

        ShaderProgram(ShaderProgram&& rhs) noexcept;
        ShaderProgram& operator=(ShaderProgram&& rhs) noexcept;

        template<typename T>
        ShaderProgram& operator<<(const UniformValue<T>& uniform) noexcept;
        ShaderProgram& operator<<(const UniformSampler& uniform) noexcept;
        ShaderProgram& operator<<(const Material& material);
    };

    void swap(ShaderProgram& lhs, ShaderProgram& rhs) noexcept;
}