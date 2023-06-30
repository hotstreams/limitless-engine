#pragma once

#include <limitless/core/indexed_buffer.hpp>

#include <vector>

namespace Limitless::ms {
    class Material;
    class MaterialCompiler;
    class MaterialBuilder;
}

namespace Limitless {
    template<typename T> class UniformValue;
    class UniformSampler;
    class Uniform;
    class Texture;
    class ContextState;

    /**
     * ShadeProgram describes compiled shader program object that is used to render object
     *
     * To create ShaderProgram use ShaderCompiler
     */
    class ShaderProgram final {
    private:
        /**
         * Unique identifier
         */
        GLuint id {};

        /**
         * Contains <uniform name, uniform location> inside the shader program
         *
         * Used for caching purposes to reduce calls to driver
         */
        std::unordered_map<std::string, GLint> locations;

        /**
         * Contains buffers data
         */
        std::vector<IndexedBufferData> indexed_binds;

        /**
         * Contains <uniform name, uniform value> inside the shader program
         *
         * Used to cache values to reduce calls to driver
         */
        std::map<std::string, std::unique_ptr<Uniform>> uniforms;

        /**
         * Binds buffer objects inside shader to current context state
         */
        void bindIndexedBuffers();

        /**
         * Binds resources in shader program
         */
        void bindResources();

        ShaderProgram() noexcept = default;
        explicit ShaderProgram(GLuint id) noexcept;

        template<typename T> friend class UniformValue;
        friend class UniformSampler;
        friend class ms::MaterialCompiler;
        friend class ShaderCompiler;
        friend class ms::MaterialBuilder;
    public:
        ~ShaderProgram();

        ShaderProgram(const ShaderProgram&) noexcept = delete;
        ShaderProgram& operator=(const ShaderProgram&) noexcept = delete;

        ShaderProgram(ShaderProgram&&) noexcept = default;
        ShaderProgram& operator=(ShaderProgram&&) noexcept = default;

        [[nodiscard]] auto getId() const noexcept { return id; }

        void use();

        ShaderProgram& setUniform(const std::string& name, std::shared_ptr<Texture> texture);
        ShaderProgram& setMaterial(const ms::Material& material);

        template<typename T>
        ShaderProgram& setUniform(const std::string& name, const T& value);
    };
}