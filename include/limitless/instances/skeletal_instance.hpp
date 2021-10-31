#pragma once

#include <limitless/instances/model_instance.hpp>
#include <limitless/instances/socket_attachment.hpp>
#include <limitless/models/skeletal_model.hpp>
#include <chrono>

namespace Limitless {
	class Buffer;

    class SkeletalInstance final : public ModelInstance, public SocketAttachment<> {
    private:
        std::vector<glm::mat4> bone_transform;
        std::shared_ptr<Buffer> bone_buffer;

        const Animation* animation {};
        bool paused {};

        std::chrono::time_point<std::chrono::steady_clock> last_time;
        std::chrono::duration<double> animation_duration;

        void updateBoundingBox() noexcept override;
        void initializeBuffer();

        void updateAnimationFrame();
        const AnimationNode* findAnimationNode(const Bone& bone) const noexcept;
    public:
        SkeletalInstance(std::shared_ptr<AbstractModel> m, const glm::vec3& position);
        ~SkeletalInstance() override = default;

        SkeletalInstance(const SkeletalInstance&) = default;
        SkeletalInstance(SkeletalInstance&&) noexcept = default;

        SkeletalInstance& setPosition(const glm::vec3& position) noexcept override;
        SkeletalInstance& setRotation(const glm::quat& rotation) noexcept override;
        SkeletalInstance& setScale(const glm::vec3& scale) noexcept override;
	    SkeletalInstance& setTransformation(const glm::mat4& transformation) override;

        SkeletalInstance* clone() noexcept override;

	    void updateAttachments(Context& context, const Camera& camera) override;
	    void update(Context& context, const Camera& camera) override;

        SkeletalInstance& play(const std::string& name);
        SkeletalInstance& pause() noexcept;
        SkeletalInstance& resume() noexcept;
        SkeletalInstance& stop() noexcept;

        const auto& getBoneTransform() const noexcept { return bone_transform; }

        // supports only IndexedMeshes for now
        glm::vec3 getSkinnedVertexPosition(const std::shared_ptr<AbstractMesh>& mesh, size_t vertex_index) const;

        using AbstractInstance::draw;
        void draw(Context& ctx, const Assets& assets, ShaderPass shader_type, ms::Blending blending, const UniformSetter& uniform_setter) override;
    };
}
