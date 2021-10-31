#pragma once

#include <limitless/util/bounding_box.hpp>
#include <limitless/instances/instance_attachment.hpp>
#include <limitless/util/matrix_stack.hpp>

namespace Limitless {
    enum class ShaderPass;
    enum class ModelShader;
    class ShaderProgram;
    class UniformSetter;
    class Assets;
    class Context;
    class Camera;

	namespace ms {
		enum class Blending;
	}

	class AbstractInstance : public InstanceAttachment {
    private:
        static inline uint64_t next_id {};
        uint64_t id;
    protected:
        ModelShader shader_type;

		glm::mat4 transformation_matrix {1.0f};
		glm::mat4 model_matrix {1.0f};
		glm::mat4 parent {1.0f};
		glm::mat4 final_matrix {1.0f};

		glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};
		glm::vec3 position {0.0f};
		glm::vec3 scale {1.0f};

		BoundingBox bounding_box {};

		bool shadow_cast {true};
		bool outlined {};
		bool hidden {};
        bool done {};

		virtual void updateBoundingBox() noexcept = 0;
		void updateModelMatrix() noexcept;
		void updateFinalMatrix() noexcept;

        AbstractInstance(ModelShader shader_type, const glm::vec3& position) noexcept;
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
        [[nodiscard]] const auto& getTransformationMatrix() const noexcept { return transformation_matrix; }
        [[nodiscard]] const auto& getFinalMatrix() const noexcept { return final_matrix; }
        [[nodiscard]] const auto& getBoundingBox() noexcept { updateBoundingBox(); return bounding_box; }

		void removeOutline() noexcept;
		void removeShadow() noexcept;
		void makeOutlined() noexcept;
		void castShadow() noexcept;
		void reveal() noexcept;
		void hide() noexcept;
		void kill() noexcept;

		[[nodiscard]] bool doesCastShadow() const noexcept;
		[[nodiscard]] bool isOutlined() const noexcept;
		[[nodiscard]] bool isHidden() const noexcept;
		[[nodiscard]] bool isKilled() const noexcept;

        virtual AbstractInstance& setPosition(const glm::vec3& position) noexcept;
        virtual AbstractInstance& setRotation(const glm::quat& rotation) noexcept;
        virtual AbstractInstance& rotateBy(const glm::quat& rotation) noexcept;
        virtual AbstractInstance& setScale(const glm::vec3& scale) noexcept;
        virtual AbstractInstance& setTransformation(const glm::mat4& transformation);
        virtual AbstractInstance& setParent(const glm::mat4& parent);

		virtual void updateAttachments(Context& context, const Camera& camera);
		virtual void update(Context& context, const Camera& camera);

        // draws instance with no extra uniform setting
        void draw(Context& ctx, const Assets& assets, ShaderPass shader_type, ms::Blending blending);

        virtual void draw(Context& ctx, const Assets& assets, ShaderPass shader_type, ms::Blending blending, const UniformSetter& uniform_set) = 0;
    };
}
