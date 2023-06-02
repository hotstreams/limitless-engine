#pragma once

#include <limitless/core/indexed_buffer.hpp>
#include <vector>
#include <limitless/shader_storage.hpp>
#include "context_state.hpp"

namespace Limitless::ms {
    class Material;
    class MaterialCompiler;
    class MaterialBuilder;
}

namespace Limitless {
    template<typename T> class UniformValue;
    class UniformSampler;
    class Uniform;
    class ContextState;

    struct shader_program_error : public std::runtime_error {
        explicit shader_program_error(const char* error) noexcept : runtime_error{error} {}
    };

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
         * Gets uniform location from map
         *
         * If uniform is not used inside the shader program code, it will be optimized out
         *
         * @param uniform - location to fetch of
         * @return - location index or -1 if not found/optimized out
         */
        GLint getUniformLocation(const Uniform& uniform) const noexcept;

        /**
         * Introspects ShaderProgram and fetches uniform locations to fill the map
         */
        void getUniformLocations() noexcept;

        /**
         * Introspects ShaderProgram and fetches uniform buffer and shader buffer storages
         *
         * @param ctx
         */
        void getIndexedBufferBounds(ContextState& ctx) noexcept;

        /**
         * Binds buffer objects inside shader to current context state
         *
         * @param ctx - state to be bound to
         */
        void bindIndexedBuffers(ContextState& ctx);

        /**
         * Binds textures used in ShaderProgram to current context state
         */
        void bindTextures() const noexcept;

        ShaderProgram() noexcept = default;
        ShaderProgram(ContextState& ctx, GLuint id);

        template<typename T> friend class UniformValue;
        friend class UniformSampler;
        friend class ms::MaterialCompiler;
        friend class ShaderCompiler;
        friend class ms::MaterialBuilder;
        friend void swap(ShaderProgram& lhs, ShaderProgram& rhs) noexcept;
    public:
        ~ShaderProgram();

        ShaderProgram(const ShaderProgram&) noexcept = delete;
        ShaderProgram& operator=(const ShaderProgram&) noexcept = delete;

        ShaderProgram(ShaderProgram&& rhs) noexcept;
        ShaderProgram& operator=(ShaderProgram&& rhs) noexcept;

        [[nodiscard]] auto getId() const noexcept { return id; }

        void use();

        template<typename T>
        ShaderProgram& operator<<(const UniformValue<T>& uniform) noexcept;
        ShaderProgram& operator<<(const UniformSampler& uniform) noexcept;
        ShaderProgram& operator<<(const ms::Material& material);
    };

    void swap(ShaderProgram& lhs, ShaderProgram& rhs) noexcept;
}