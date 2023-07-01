#pragma once

#include <limitless/core/uniform/uniform.hpp>
#include <limitless/core/uniform/uniform_value.hpp>
#include <limitless/core/uniform/uniform_sampler.hpp>
#include <limitless/core/uniform/uniform_time.hpp>
#include <limitless/pipeline/shader_pass_types.hpp>
#include <limitless/ms/property.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/ms/shading.hpp>

#include <unordered_map>
#include <glm/glm.hpp>
#include <stdexcept>
#include <vector>
#include <map>

namespace Limitless {
    class Buffer;
}

namespace Limitless::ms {
    class material_property_not_found : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    /**
     * Material class describes visual look of a surface
     *
     * Can be applied to meshes, models and effects
     *
     * Should be compiled using MaterialCompiler to generate shader for usage
     */
    class Material final {
    private:
        /**
         * Contains set of predefined properties
         *
         * values of properties can be changed at run-time
         */
        std::map<Property, std::unique_ptr<Uniform>> properties;

        /**
         *  Describes material blending mode
         *
         *  can be changed at run-time
         */
        Blending blending {};

        /**
         *  Describes material shading model
         *
         *  compile-time only
         */
        Shading shading {};

        /**
         *  Describes whether material is two sided
         */
        bool two_sided {};

        /**
         *  Describes whether material is using refraction
         */
        bool refraction {};

        /**
         * Unique material name
         */
        std::string name;

        /**
         * Describes which shader is compiled for this material
         */
        uint64_t shader_index {};

        /**
         * Describes for which ModelShader types this material is used and compiled
         */
        ModelShaders model_shaders;

        /**
         * Corresponding OpenGL buffer to store properties on GPU
         */
        std::shared_ptr<Buffer> material_buffer;

        /**
         * Run-time fetched property offsets in buffer
         */
        std::unordered_map<std::string, uint64_t> uniform_offsets;

        /**
         * Custom user-defined properties
         */
        std::map<std::string, std::unique_ptr<Uniform>> uniforms;

        /**
         * Vertex shader code to allow customization
         */
        std::string vertex_snippet;

        /**
         * Fragment shader code to allow customization
         */
        std::string fragment_snippet;

        /**
         * Global shader code to allow customization
         */
        std::string global_snippet;

        /**
         * Tessellation shader code to allow customization
         */
        std::string tessellation_snippet;

        /**
         * Adds uniform typed value at specific offset in block so it can be mapped to GPU
         */
        template<typename V>
        void map(std::vector<std::byte>& block, const Uniform& uniform) const;

        /**
        * Adds any uniform to block
        */
        void map(std::vector<std::byte>& block, Uniform& uniform);

        /**
         * Maps material to GPU uniform buffer
         */
        void map();

        friend void swap(Material&, Material&) noexcept;
        Material() = default;

        friend class MaterialBuilder;

        /**
         * Equality operators to determine whether two materials could be used by the same shader
         */
        friend bool operator==(const Material& lhs, const Material& rhs) noexcept;
        friend bool operator<(const Material& lhs, const Material& rhs) noexcept;
    public:
        ~Material() = default;

        /**
         * Material copy constructor
         *
         * makes material deep copy
         */
        Material(const Material&);

        /**
         * Material copy assignment operator
         *
         * makes material deep copy
         */
        Material& operator=(Material);

        Material(Material&&) noexcept = default;
        Material& operator=(Material&&) noexcept = default;

        /**
         *  Maps material to uniform buffer
         */
        void update();

        [[nodiscard]] const UniformValue<glm::vec4>& getColor() const;
        [[nodiscard]] const UniformValue<glm::vec4>& getEmissiveColor() const;
        [[nodiscard]] const UniformValue<glm::vec2>& getTesselationFactor() const;
        [[nodiscard]] const UniformValue<float>& getMetallic() const;
        [[nodiscard]] const UniformValue<float>& getRoughness() const;
        [[nodiscard]] const UniformValue<float>& getIoR() const;
        [[nodiscard]] const UniformValue<float>& getAbsorption() const;
        [[nodiscard]] const UniformSampler& getMetallicTexture() const;
        [[nodiscard]] const UniformSampler& getRoughnessTexture() const;
        [[nodiscard]] const UniformSampler& getDiffuse() const;
        [[nodiscard]] const UniformSampler& getNormal() const;
        [[nodiscard]] const UniformSampler& getEmissiveMask() const;
        [[nodiscard]] const UniformSampler& getBlendMask() const;
        [[nodiscard]] const UniformSampler& getAmbientOcclusionTexture() const;

        UniformValue<glm::vec4>& getColor();
        UniformValue<glm::vec4>& getEmissiveColor();
        UniformValue<glm::vec2>& getTesselationFactor();
        UniformValue<float>& getMetallic();
        UniformValue<float>& getRoughness();
        UniformValue<float>& getIoR();
        UniformValue<float>& getAbsorption();
        UniformSampler& getMetallicTexture();
        UniformSampler& getRoughnessTexture();
        UniformSampler& getDiffuse();
        UniformSampler& getNormal();
        UniformSampler& getEmissiveMask();
        UniformSampler& getBlendMask();
        UniformSampler& getAmbientOcclusionTexture();

        [[nodiscard]] const auto& getModelShaders() const noexcept { return model_shaders; }
        [[nodiscard]] auto getBlending() const noexcept { return blending; }
        [[nodiscard]] auto getShading() const noexcept { return shading; }
        [[nodiscard]] auto getTwoSided() const noexcept { return two_sided; }
        [[nodiscard]] const auto& getName() const noexcept { return name; }
        [[nodiscard]] auto getShaderIndex() const noexcept { return shader_index; }
        [[nodiscard]] const auto& getVertexSnippet() const noexcept { return vertex_snippet; }
        [[nodiscard]] const auto& getRefraction() const noexcept { return refraction; }
        [[nodiscard]] const auto& getFragmentSnippet() const noexcept { return fragment_snippet; }
        [[nodiscard]] const auto& getGlobalSnippet() const noexcept { return global_snippet; }
        [[nodiscard]] const auto& getTessellationSnippet() const noexcept { return tessellation_snippet; }
        [[nodiscard]] const auto& getMaterialBuffer() const noexcept { return material_buffer; }
        [[nodiscard]] const auto& getProperties() const noexcept { return properties; }
        [[nodiscard]] const auto& getUniforms() const noexcept { return uniforms; }

        auto& getBlending() noexcept { return blending; }
        auto& getTwoSided() noexcept { return two_sided; }

        bool contains(Property property) const;
        bool contains(const std::string& name) const;

        template<typename Uniform>
        Uniform& getProperty(const std::string& uniform_name) {
            try {
                return static_cast<Uniform&>(*uniforms.at(uniform_name));
            } catch (...) {
                throw material_property_not_found(uniform_name);
            }
        }

        template<typename Uniform>
        const Uniform& getProperty(const std::string& uniform_name) const {
            try {
                return static_cast<const Uniform&>(*uniforms.at(uniform_name));
            } catch (...) {
                throw material_property_not_found(uniform_name);
            }
        }
    };

    void swap(Material& lhs, Material& rhs) noexcept;

    bool operator==(const Material& lhs, const Material& rhs) noexcept;
    bool operator<(const Material& lhs, const Material& rhs) noexcept;
}