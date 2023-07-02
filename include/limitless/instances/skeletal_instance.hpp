#pragma once

#include <limitless/instances/model_instance.hpp>
#include <limitless/instances/socket_attachment.hpp>
#include <limitless/models/skeletal_model.hpp>
#include <chrono>

namespace Limitless {
    class Buffer;

    class no_such_animation : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };
}

namespace Limitless {
    /**
     * Skeletal Instance that have bones and animations
     */
    class SkeletalInstance final : public ModelInstance, public SocketAttachment {
    private:
        /**
         * Bones transformation matrices
         */
        std::vector<glm::mat4> bone_transform;

        /**
         * OpenGL buffer for bone transformations
         */
        std::shared_ptr<Buffer> bone_buffer;

        /**
         * Current animation
         */
        const Animation* animation {};

        /**
         * Is animation paused
         */
        bool paused {};

        /**
         * Animation last update time
         */
        std::chrono::time_point<std::chrono::steady_clock> last_time;

        /**
         * Current animation ongoing duration
         */
        std::chrono::duration<double> animation_duration {};

        void initializeBuffer();

        /**
         * Updates bone transformation for current animation frame
         */
        void updateAnimationFrame();
        const AnimationNode* findAnimationNode(const Bone& bone) const noexcept;
    public:
        /**
         * Creates instance with SkeletalModel
         */
        SkeletalInstance(std::shared_ptr<AbstractModel> m, const glm::vec3& position);
        ~SkeletalInstance() override = default;

        SkeletalInstance(const SkeletalInstance&) noexcept;
        SkeletalInstance(SkeletalInstance&&) noexcept = default;

        /**
         * Makes instance copy
         */
        std::unique_ptr<AbstractInstance> clone() noexcept override;

        /**
         * Updates current animation, socket attachments data and instance itself
         */
	    void update(Context& context, const Camera& camera) override;

        /**
         * Plays animation with name
         *
         * throws no_such_animation if not found
         */
        SkeletalInstance& play(const std::string& name);

        /**
         * Pauses current animation
         */
        SkeletalInstance& pause() noexcept;

        /**
         * Resumes current animation
         */
        SkeletalInstance& resume() noexcept;

        /**
         * Stops current animation
         */
        SkeletalInstance& stop() noexcept;

        auto isPaused() const noexcept { return paused; }
        const auto& getCurrentAnimation() const noexcept { return animation; }
        const std::vector<Animation>& getAllAnimations() const noexcept;
        const std::vector<Bone>& getAllBones() const noexcept;
        /**
         * Calculates transformed vertex position on specified instance mesh for specified vertex
         */
        glm::vec3 getSkinnedVertexPosition(const std::shared_ptr<AbstractMesh>& mesh, size_t vertex_index) const;

        const auto& getBoneTransform() const noexcept { return bone_transform; }

        using AbstractInstance::draw;
        void draw(Context& ctx, const Assets& assets, ShaderPass shader_type, ms::Blending blending, const UniformSetter& uniform_setter) override;
    };
}
