#pragma once

#include <limitless/instances/effect_attachable.hpp>
#include <limitless/instances/light_attachable.hpp>
#include <limitless/util/bounding_box.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Limitless::ms {
    enum class Blending;
}

namespace Limitless {
    enum class ShaderPass;
    enum class ModelShader;
    class ShaderProgram;
    class UniformSetter;
    class Assets;
    class Context;
    class Camera;

    class AbstractInstance : public EffectAttachable, public LightAttachable {
    private:
        static inline uint64_t next_id {};
        uint64_t id;
    protected:
        ModelShader shader_type;
        bool done {};
        bool hidden {};
        bool shadow_cast {true};

        glm::vec3 position;
        glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 scale;
        glm::mat4 model_matrix {1.0f};

        BoundingBox bounding_box {};

        virtual void calculateBoundingBox() noexcept = 0;
        void calculateModelMatrix() noexcept;

        AbstractInstance(Lighting* lighting, ModelShader shader_type, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) noexcept;
        AbstractInstance(ModelShader shader_type, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) noexcept;
    public:
        ~AbstractInstance() override = default;

        AbstractInstance(const AbstractInstance&) = default;
        AbstractInstance(AbstractInstance&&) = default;

        virtual AbstractInstance* clone() noexcept = 0;

        [[nodiscard]] auto getShaderType() const noexcept { return shader_type; }
        [[nodiscard]] auto getId() const noexcept { return id; }

        [[nodiscard]] const auto& getPosition() const noexcept { return position; }
        [[nodiscard]] const auto& getRotation() const noexcept { return rotation; }
        [[nodiscard]] const auto& getScale() const noexcept { return scale; }
        [[nodiscard]] const auto& getModelMatrix() const noexcept { return model_matrix; }
        [[nodiscard]] const auto& getBoundingBox() noexcept { calculateBoundingBox(); return bounding_box; }

        void reveal() noexcept;
        void hide() noexcept;
	    [[nodiscard]] bool isHidden() const noexcept { return hidden; }

        [[nodiscard]] bool isKilled() const noexcept;
        void kill() noexcept;

        [[nodiscard]] bool doesCastShadow() const noexcept { return shadow_cast; }
        void castShadow() noexcept { shadow_cast = true; }
        void removeShadow() noexcept { shadow_cast = false; }

        void update(Context& context, Camera& camera) override;

        virtual AbstractInstance& setPosition(const glm::vec3& position) noexcept;
        virtual AbstractInstance& setRotation(const glm::quat& rotation) noexcept;
        virtual AbstractInstance& rotateBy(const glm::quat& rotation) noexcept;
        virtual AbstractInstance& setScale(const glm::vec3& scale) noexcept;

        // draws instance with no extra uniform setting
        void draw(Context& ctx, const Assets& assets, ShaderPass shader_type, ms::Blending blending);

        virtual void draw(Context& ctx, const Assets& assets, ShaderPass shader_type, ms::Blending blending, const UniformSetter& uniform_set) = 0;
    };
}
