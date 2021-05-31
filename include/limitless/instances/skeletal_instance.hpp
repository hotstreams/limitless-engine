#pragma once

#include <limitless/instances/model_instance.hpp>
#include <limitless/models/skeletal_model.hpp>

namespace Limitless {
    class SkeletalInstance final : public ModelInstance {
    private:
        std::vector<glm::mat4> bone_transform;
        std::shared_ptr<Buffer> bone_buffer;

        const Animation* animation {};
        bool paused {};

        void calculateBoundingBox() noexcept override;
        void initializeBuffer();

        const AnimationNode* findAnimationNode(const Bone& bone) const noexcept;
    public:
        SkeletalInstance(std::shared_ptr<AbstractModel> m, const glm::vec3& position);
        SkeletalInstance(Lighting* lighting, std::shared_ptr<AbstractModel> m, const glm::vec3& position);
        ~SkeletalInstance() override = default;

        SkeletalInstance(const SkeletalInstance&) = default;
        SkeletalInstance(SkeletalInstance&&) noexcept = default;

        SkeletalInstance& setPosition(const glm::vec3& position) noexcept override;
        SkeletalInstance& setRotation(const glm::quat& rotation) noexcept override;
        SkeletalInstance& setScale(const glm::vec3& scale) noexcept override;

        SkeletalInstance* clone() noexcept override;

        void update(Context& context, Camera& camera) override;

        SkeletalInstance& play(const std::string& name);
        SkeletalInstance& pause() noexcept;
        SkeletalInstance& resume() noexcept;
        SkeletalInstance& stop() noexcept;

        // supports only IndexedMeshes for now
        glm::vec3 getSkinnedVertexPosition(const std::shared_ptr<AbstractMesh>& mesh, size_t vertex_index) const;

//        void attachUpdate() {
////            constexpr auto bone_name = "head_end";
//            constexpr auto bone_name = "weapon_r";
//            auto& skeletal = dynamic_cast<SkeletalModel&>(*model);
//            const auto& map = skeletal.getBoneMap();
//            for (const auto& item : map) {
//                std::cout << item.first << std::endl;
//            }
//
//            const auto index = map.at(bone_name);
//
//            attachment->getModelMatrix() = model_matrix * bone_transform.at(index) * glm::inverse(skeletal.getBones().at(index).offset_matrix);
//        }

        using AbstractInstance::draw;
        void draw(Context& ctx, const Assets& assets, ShaderPass shader_type, ms::Blending blending, const UniformSetter& uniform_setter) override;
    };
}
