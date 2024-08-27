#pragma once

#include <limitless/core/uniform/uniform.hpp>
#include <limitless/renderer/shader_type.hpp>
#include <limitless/ms/property.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/ms/shading.hpp>

#include <glm/glm.hpp>
#include <stdexcept>
#include <map>
#include <unordered_map>
#include <memory>
#include <string>
#include <cstdint>
#include <cstddef>
#include <vector>

namespace Limitless {
    class Buffer;
    class Texture;
}

namespace Limitless::ms {
    class material_exception : public std::runtime_error {
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
        Blending blending;

        /**
         *  Describes material shading model
         *
         *  compile-time only
         */
        Shading shading;

        /**
         *  Describes whether material is two sided
         */
        bool two_sided;

        /**
         *  Describes whether material is using refraction
         */
        bool refraction;

        /**
         *  Unique material name
         */
        std::string name;

        /**
         *  Describes which shader is compiled for this material
         */
        uint64_t shader_index;

        /**
         *  Describes for which ModelShader types this material is used and compiled
         */
        InstanceTypes model_shaders;

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
         * Shading shader code to allow customization
         */
        std::string shading_snippet;

        /**
         * Material Buffer
         */
        class Buffer final {
        private:
            /**
             *  Corresponding OpenGL buffer to store properties on GPU
             */
            std::shared_ptr<Limitless::Buffer> material_buffer;

            /**
             *  Property offsets in buffer
             */
            std::unordered_map<std::string, uint64_t> uniform_offsets;

            /**
             *  initializes material offsets in buffer
             */
            void initialize(const Material& material);

            /**
            *   Adds uniform typed value at specific offset in block so it can be mapped to GPU
            */
            template<typename V>
            void map(std::vector<std::byte>& block, const Uniform& uniform) const;

            /**
            *   Adds any uniform to block
            */
            void map(std::vector<std::byte>& block, Uniform& uniform);
        public:
            explicit Buffer(const Material& material);

            Buffer(const Buffer& buffer);
            Buffer& operator=(const Buffer& buffer);

            Buffer(Buffer&&) noexcept = default;
            Buffer& operator=(Buffer&&) noexcept = default;

            const std::shared_ptr<Limitless::Buffer>& getBuffer() const noexcept;

            /**
             *  Maps material to GPU uniform buffer
             */
            void map(const Material& material);
        };
        Buffer buffer;

        /**
         *  For copy and swap idiom
         */
        friend void swap(Material&, Material&) noexcept;

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
         *  Updates material
         */
        void update();

        /**
         * Const-context property getters
         */
        [[nodiscard]] const glm::vec4& getColor() const;
        [[nodiscard]] const glm::vec3& getEmissiveColor() const;
        [[nodiscard]] float getMetallic() const;
        [[nodiscard]] float getRoughness() const;
        [[nodiscard]] float getIoR() const;
        [[nodiscard]] float getAbsorption() const;
        [[nodiscard]] float getMicroThickness() const;
        [[nodiscard]] float getThickness() const;
        [[nodiscard]] float getReflectance() const;
        [[nodiscard]] float getTransmission() const;
        [[nodiscard]] const std::shared_ptr<Texture>& getDiffuseTexture() const;
        [[nodiscard]] const std::shared_ptr<Texture>& getNormalTexture() const;
        [[nodiscard]] const std::shared_ptr<Texture>& getEmissiveMaskTexture() const;
        [[nodiscard]] const std::shared_ptr<Texture>& getBlendMaskTexture() const;
        [[nodiscard]] const std::shared_ptr<Texture>& getMetallicTexture() const;
        [[nodiscard]] const std::shared_ptr<Texture>& getRoughnessTexture() const;
        [[nodiscard]] const std::shared_ptr<Texture>& getAmbientOcclusionTexture() const;
        [[nodiscard]] const std::shared_ptr<Texture>& getORMTexture() const;

        /**
         * Mutable property getters for primitives
         */
        glm::vec4& getColor();
        glm::vec3& getEmissiveColor();
        float& getMetallic();
        float& getRoughness();
        float& getIoR();
        float& getAbsorption();
        float& getMicroThickness();
        float& getThickness();
        float& getReflectance();
        float& getTransmission();
        Blending& getBlending();

        /**
         * Property setters
         */
        void setColor(const glm::vec4& value);
        void setEmissiveColor(const glm::vec3& value);
        void setMetallic(float value);
        void setRoughness(float value);
        void setIoR(float value);
        void setAbsorption(float value);
        void setMicroThickness(float value);
        void setThickness(float value);
        void setReflectance(float value);
        void setTransmission(float value);
        void setDiffuseTexture(const std::shared_ptr<Texture>& texture);
        void setNormalTexture(const std::shared_ptr<Texture>& texture);
        void setEmissiveMaskTexture(const std::shared_ptr<Texture>& texture);
        void setBlendMaskTexture(const std::shared_ptr<Texture>& texture);
        void setMetallicTexture(const std::shared_ptr<Texture>& texture);
        void setRoughnessTexture(const std::shared_ptr<Texture>& texture);
        void setAmbientOcclusionTexture(const std::shared_ptr<Texture>& texture);
        void setORMTexture(const std::shared_ptr<Texture>& texture);
        void setBlending(Blending blending);

        [[nodiscard]] Blending getBlending() const noexcept;
        [[nodiscard]] Shading getShading() const noexcept;
        [[nodiscard]] bool getTwoSided() const noexcept;
        [[nodiscard]] bool getRefraction() const noexcept;
        [[nodiscard]] const std::string& getName() const noexcept;
        [[nodiscard]] uint64_t getShaderIndex() const noexcept;
        [[nodiscard]] const std::string& getVertexSnippet() const noexcept;
        [[nodiscard]] const std::string& getFragmentSnippet() const noexcept;
        [[nodiscard]] const std::string& getGlobalSnippet() const noexcept;
        [[nodiscard]] const std::string& getShadingSnippet() const noexcept;
        [[nodiscard]] const InstanceTypes& getModelShaders() const noexcept;
        [[nodiscard]] const Buffer& getBuffer() const noexcept;


        [[nodiscard]] const std::map<Property, std::unique_ptr<Uniform>>& getProperties() const noexcept;
        [[nodiscard]] const std::map<std::string, std::unique_ptr<Uniform>>& getUniforms() const noexcept;

        /**
         *  Material Builder
         */
        class Builder;

        /**
         *  Returns new instance of material builder
         */
        static Builder builder();

        private:
            /**
             *  Constructs material from builder
             */
            explicit Material(Builder& builder);
    };

    void swap(Material& lhs, Material& rhs) noexcept;

    bool operator==(const Material& lhs, const Material& rhs) noexcept;
    bool operator<(const Material& lhs, const Material& rhs) noexcept;
}
