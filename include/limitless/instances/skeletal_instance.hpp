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
         * Bone world-space matrices (hierarchy global transform, indexed by bone index)
         */
        std::vector<glm::mat4> bone_world_transform;

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
         * Is animation repeating
         */
        bool repeating {true};

        /**
         * Playback speed multiplier (1 = real time). 0 freezes the clip.
         */
        float playback_speed {1.f};

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
        [[nodiscard]] const AnimationNode* findAnimationNode(const Bone& bone) const noexcept;
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
        std::unique_ptr<Instance> clone() noexcept override;

        /**
         * Updates current animation, socket attachments data and instance itself
         */
        void update(const Camera &camera) override;

        /**
         * Plays animation with name
         *
         * throws no_such_animation if not found
         */
        SkeletalInstance& play(const std::string& name);
        SkeletalInstance& play(uint32_t index);

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

        /**
         * Sets animation repeating
         */
        SkeletalInstance& setRepeating(bool repeating) noexcept;

        /**
         * Playback speed relative to wall-clock time. 1 is real time; 0 freezes.
         */
        SkeletalInstance& setPlaybackSpeed(float speed) noexcept;
        [[nodiscard]] auto getPlaybackSpeed() const noexcept { return playback_speed; }

        /**
         * Current animation time in ticks (duration.count() * tps, wrapped/clamped)
         */
        [[nodiscard]] double getAnimationTime() const noexcept;
        SkeletalInstance& setAnimationTime(double ticks) noexcept;

        [[nodiscard]] auto isPaused() const noexcept { return paused; }
        [[nodiscard]] auto isRepeating() const noexcept { return repeating; }
        [[nodiscard]] const auto& getCurrentAnimation() const noexcept { return animation; }
        [[nodiscard]] const std::vector<Animation>& getAllAnimations() const noexcept;
        [[nodiscard]] bool hasAnimation(const std::string& name) const noexcept;
        const std::vector<Bone>& getAllBones() const noexcept;
        const auto& getBoneBuffer() const noexcept { return bone_buffer; }
        const auto& getBoneWorldMatrices() const noexcept { return bone_world_transform; }

        /**
         * Calculates transformed vertex position on specified instance mesh for specified vertex
         */
        [[nodiscard]] glm::vec3 getSkinnedVertexPosition(const std::shared_ptr<AbstractMesh>& mesh, size_t vertex_index) const;

        const auto& getBoneTransform() const noexcept { return bone_transform; }
    };
}
