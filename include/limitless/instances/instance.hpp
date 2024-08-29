#pragma once

#include <limitless/util/box.hpp>
#include <limitless/instances/instance_attachment.hpp>
#include <limitless/util/matrix_stack.hpp>
#include <limitless/util/frustum.hpp>
#include <optional>

namespace Limitless {
    enum class ShaderType;
    enum class InstanceType;
    class ShaderProgram;
    class UniformSetter;
    class Assets;
    class Context;
    class Camera;

    namespace ms {
        enum class Blending;
    }
}

namespace Limitless {
    /**
     * AbstractInstance is a base class that provides basic functionality for rendering objects
     */
	class Instance : public InstanceAttachment {
    private:
        static inline uint64_t next_id {1};
        /**
         * Unique instance identifier
         */
        uint64_t id;
    protected:
        /**
         * Instance type
         */
        InstanceType shader_type;

        /**
         * Matrices used to calculate transformation among hierarchy of different instances
         */

        /**
         * Current instance final transformation
         */
        glm::mat4 final_matrix {1.0f};

        /**
         * Matrix of parent's final transformation
         */
        glm::mat4 parent {1.0f};

        /**
         * Current instance skeletal-transformation matrix
         *
         * Used if this instance attached to bone
         */
        glm::mat4 transformation_matrix {1.0f};

        /**
         * Current instance translation-rotation-scale matrix
         */
        glm::mat4 model_matrix {1.0f};

        /**
         * Rotation quaternion
         */
		glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};

        /**
         * World instance position
         */
		glm::vec3 position {0.0f};

        /**
         * Instance scale
         */
		glm::vec3 scale {1.0f};

        /**
         * Final bounding box
         */
		Box bounding_box {};

        /**
         * User-set bounding box
         */
        std::optional<Box> custom_bounding_box {};

        /**
         * Instance decal mask
         */
        uint8_t decal_mask {0x00};

        /**
         * Does instance cast shadow
         */
		bool shadow_cast {true};

        /**
         * Does instance outlined
         */
		bool outlined {};

        /**
         * Does instance to be rendered
         */
		bool hidden {};

        /**
         * Whether instance should be removed from scene in the next update
         */
        bool done {};

        /**
         * Whether instance can be pickable from ColorPicker
         */
        bool pickable {};

        /**
         * Default implementation of bounding box updates sets custom user box if present
         */
		virtual void updateBoundingBox() noexcept;

		void updateModelMatrix() noexcept;
		void updateFinalMatrix() noexcept;

        Instance(InstanceType shader_type, const glm::vec3& position) noexcept;
    public:
        ~Instance() override = default;

        Instance(const Instance&);
        Instance(Instance&&) = default;

        /**
         * Makes instance copy
         */
        virtual std::unique_ptr<Instance> clone() noexcept = 0;

        [[nodiscard]] auto getInstanceType() const noexcept { return shader_type; }
        [[nodiscard]] auto getId() const noexcept { return id; }

        [[nodiscard]] const auto& getPosition() const noexcept { return position; }
        [[nodiscard]] const auto& getRotation() const noexcept { return rotation; }
        [[nodiscard]] const auto& getScale() const noexcept { return scale; }

        [[nodiscard]] const auto& getTransformationMatrix() const noexcept { return transformation_matrix; }
        [[nodiscard]] const auto& getBoundingBox() noexcept { updateBoundingBox(); return bounding_box; }
        [[nodiscard]] const auto& getFinalMatrix() const noexcept { return final_matrix; }
        [[nodiscard]] const auto& getModelMatrix() const noexcept { return model_matrix; }

        [[nodiscard]] const auto& getDecalMask() const noexcept { return decal_mask; }

        /**
         * Instance outlined
         */
        void makeOutlined() noexcept;

        /**
         * Instance not outlined
         */
		void removeOutline() noexcept;

        /**
         * Casts shadow
         */
        void castShadow() noexcept;

        /**
         * Instance does not cast shadow
         */
		void removeShadow() noexcept;

        /**
         * Instance does not get rendered
         */
        void hide() noexcept;

        /**
         * Instance does get rendered
         */
		void reveal() noexcept;

        /**
         * Makes instance removed in next frame
         */
		void kill() noexcept;

        /**
         * Makes instance pickable for ColorPicker
         */
        void makePickable() noexcept;

        /**
         * Removes picking from ColorPicker
         */
        void removePicking() noexcept;

		[[nodiscard]] bool doesCastShadow() const noexcept;
		[[nodiscard]] bool isOutlined() const noexcept;
		[[nodiscard]] bool isHidden() const noexcept;
		[[nodiscard]] bool isKilled() const noexcept;
		[[nodiscard]] bool isPickable() const noexcept;

        /**
         * Sets instance absolute position
         */
        virtual Instance& setPosition(const glm::vec3& position) noexcept;

        /**
         * Sets instance absolute rotation
         */
        virtual Instance& setRotation(const glm::quat& rotation) noexcept;

        /**
         * Rotates instance with specified amount
         */
        virtual Instance& rotateBy(const glm::quat& rotation) noexcept;

        /**
         * Sets instance absolute scale
         */
        virtual Instance& setScale(const glm::vec3& scale) noexcept;

        /**
         * Sets current instance transformation matrix
         *
         * This is skeletal-transformation matrix used in bone attachments
         */
        virtual Instance& setTransformation(const glm::mat4& transformation);

        /**
         * Sets current instance parent matrix
         */
        virtual Instance& setParent(const glm::mat4& parent) noexcept;

        /**
         * Sets custom bounding box
         */
        virtual Instance& setBoundingBox(const Box& box) noexcept;

        /**
         * Sets decal mask
         */
        virtual Instance& setDecalMask(uint8_t mask) noexcept;

        /**
         * Updates instance data
         */
		virtual void update(const Camera &camera);

        /**
         *  Instance builder
         */
        class Builder;

        static Builder builder() noexcept;
    };
}
