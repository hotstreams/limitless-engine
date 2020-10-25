#pragma once

#include <effect_attachable.hpp>
#include <light_attachable.hpp>
#include <functional>

namespace GraphicsEngine {
    enum class MaterialShader;
    enum class ModelShader;
    enum class Blending;
    class ShaderProgram;

    class AbstractInstance : public EffectAttachable, public LightAttachable {
    private:
        static inline uint64_t next_id {0};
        uint64_t id;
    protected:
        ModelShader shader_type;
        bool done {false};
        bool hidden {false};
        bool wireframe {false};

        glm::vec3 position, rotation, scale;

        AbstractInstance(Lighting* lighting, ModelShader shader_type, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) noexcept;
        AbstractInstance(ModelShader shader_type, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) noexcept;
    public:
        ~AbstractInstance() override = default;

        AbstractInstance(const AbstractInstance&) = default;
        AbstractInstance& operator=(const AbstractInstance&) = default;

        AbstractInstance(AbstractInstance&&) = default;
        AbstractInstance& operator=(AbstractInstance&&) = default;

        virtual AbstractInstance* clone() noexcept = 0;

        [[nodiscard]] auto getShaderType() const noexcept { return shader_type; }
        [[nodiscard]] auto isWireFrame() const noexcept { return wireframe; }
        [[nodiscard]] auto getId() const noexcept { return id; }

        [[nodiscard]] const auto& getPosition() const noexcept { return position; }
        [[nodiscard]] const auto& getRotation() const noexcept { return rotation; }
        [[nodiscard]] const auto& getScale() const noexcept { return scale; }

        void reveal() noexcept;
        void hide() noexcept;

        [[nodiscard]] bool isKilled() const noexcept;
        void kill() noexcept;

        void asWireFrame() noexcept;
        void asModel() noexcept;

        virtual void update();

        virtual AbstractInstance& setPosition(const glm::vec3& position) noexcept;
        virtual AbstractInstance& setRotation(const glm::vec3& rotation) noexcept;
        virtual AbstractInstance& setScale(const glm::vec3& scale) noexcept;

        // draws instance with no extra uniform setting
        void draw(MaterialShader shader_type, Blending blending);

        using UniformSetter = std::vector<std::function<void(ShaderProgram&)>>;
        virtual void draw(MaterialShader shader_type, Blending blending, const UniformSetter& uniform_set) = 0;
    };
}
