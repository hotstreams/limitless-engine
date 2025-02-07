#pragma once

#include <limitless/models/model.hpp>
#include <limitless/util/tree.hpp>
#include <limitless/models/bones.hpp>
#include <glm/gtx/quaternion.hpp>
#include <unordered_map>
#include <optional>

namespace Limitless {
    template <typename T>
    struct KeyFrame {
        T data;
        double time;

        KeyFrame(T data, double time) noexcept
            : data{std::move(data)}
            , time(time) {}
    };

    struct AnimationNode {
        std::vector<KeyFrame<glm::fquat>> rotations;
        std::vector<KeyFrame<glm::vec3>> positions;
        std::vector<KeyFrame<glm::vec3>> scales;
        Bone &bone;

        AnimationNode(decltype(positions) positions, decltype(rotations) rotations, decltype(scales) scales, Bone &bone) noexcept;

        [[nodiscard]] size_t findPositionKeyframe(double anim_time) const;
        [[nodiscard]] size_t findRotationKeyframe(double anim_time) const;
        [[nodiscard]] size_t findScalingKeyframe(double anim_time) const;
        [[nodiscard]] std::optional<glm::vec3> positionLerp(double anim_time) const;
        [[nodiscard]] std::optional<glm::fquat> rotationLerp(double anim_time) const;
        [[nodiscard]] std::optional<glm::vec3> scalingLerp(double anim_time) const;
    };

    struct Animation {
        std::vector<AnimationNode> nodes;
        std::string name;
        double duration;
        double tps;

        Animation(std::string name, double duration, double tps, decltype(nodes) nodes) noexcept
            : nodes(std::move(nodes))
            , name(std::move(name))
            , duration(duration)
            , tps(tps) {
        }
    };

    class SkeletalModel : public Model {
    protected:
        std::unordered_map<std::string, uint32_t> bone_map;
        std::vector<Animation> animations;
        std::vector<Bone> bones;
        std::vector<Tree<uint32_t>> skeletons;

        friend class Builder;

        SkeletalModel(
            decltype(meshes)&& meshes,
            decltype(materials)&& materials,
            decltype(bones)&& bones,
            decltype(bone_map)&& bone_map,
            decltype(skeletons)&& skeletons,
            decltype(animations)&& a,
            std::string name
        ) noexcept;
    public:
        ~SkeletalModel() override = default;

        SkeletalModel(const SkeletalModel&) = delete;
        SkeletalModel& operator=(const SkeletalModel&) = delete;

        [[nodiscard]] const auto& getAnimations() const noexcept { return animations; }
        [[nodiscard]] const auto& getSkeletonTrees() const noexcept { return skeletons; }
        [[nodiscard]] const auto& getBones() const noexcept { return bones; }

        auto& getSkeletonTrees() noexcept { return skeletons; }
        auto& getAnimations() noexcept { return animations; }
        auto& getBoneMap() noexcept { return bone_map; }
        const auto& getBoneMap() const noexcept { return bone_map; }
        auto& getBones() noexcept { return bones; }
    };
}