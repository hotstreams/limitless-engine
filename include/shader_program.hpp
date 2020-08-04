#pragma once

#include <material.hpp>

namespace GraphicsEngine {
    struct IndexedBufferData {
        IndexedBuffer::Type target;
        std::string name;
        GLint block_index {0};
        GLuint bound_point {0};
        bool index_connected {false};
    };

    class ShaderProgram {
    private:
        GLuint id;
        // stores uniform locations inside the shader
        std::unordered_map<std::string, GLint> locations;
        // stores indexed buffers binding data
        std::vector<IndexedBufferData> indexed_binds;
        // stores uniform values
        std::map<std::string, std::unique_ptr<Uniform>> uniforms;

        void setUniform(const UniformValue<glm::vec2>& uniform) const;
        void setUniform(const UniformValue<glm::vec3>& uniform) const;
        void setUniform(const UniformValue<glm::vec4>& uniform) const;
        void setUniform(const UniformValue<glm::mat3>& uniform) const;
        void setUniform(const UniformValue<glm::mat4>& uniform) const;
        void setUniform(const UniformValue<unsigned int>& uniform) const;
        void setUniform(const UniformValue<float>& uniform) const;
        void setUniform(const UniformValue<int>& uniform) const;
        void setUniform(const UniformSampler& sampler) const;

        void getUniformLocations() noexcept;
        void getIndexedBufferBounds() noexcept;
        void bindIndexedBuffers();

        ShaderProgram();
        explicit ShaderProgram(GLuint id);

        template<typename T> friend class UniformValue;
        friend class UniformSampler;
        friend class ShaderCompiler;
        friend class MaterialBuilder;
        friend void swap(ShaderProgram& lhs, ShaderProgram& rhs) noexcept;
    public:
        ~ShaderProgram();

        void use();

        [[nodiscard]] auto getId() const noexcept { return id; }

        ShaderProgram(const ShaderProgram&) noexcept = delete;
        ShaderProgram& operator=(const ShaderProgram&) const noexcept = delete;

        ShaderProgram(ShaderProgram&& rhs) noexcept;
        ShaderProgram& operator=(ShaderProgram&& rhs) noexcept;

        template<typename T>
        ShaderProgram& operator<<(const UniformValue<T>& uniform) noexcept;
        ShaderProgram& operator<<(const UniformSampler& uniform) noexcept;
        ShaderProgram& operator<<(const Material& material);
    };

    void swap(ShaderProgram& lhs, ShaderProgram& rhs) noexcept;
}