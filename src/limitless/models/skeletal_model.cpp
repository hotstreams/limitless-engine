#include <limitless/models/skeletal_model.hpp>
#include <stdexcept>

using namespace Limitless;

AnimationNode::AnimationNode(decltype(positions) _positions, decltype(rotations) _rotations, decltype(scales) _scales, Bone& _bone) noexcept
    : rotations(std::move(_rotations))
    , positions(std::move(_positions))
    , scales(std::move(_scales))
    , bone(_bone) {
}

size_t AnimationNode::findPositionKeyframe(double anim_time) const {
    for (size_t i = 0; i < positions.size() - 1; ++i) {
        if (anim_time <= positions[i + 1].time) {
            return i;
        }
    }
    return 0;
    throw std::out_of_range("no position keyframe for time " + std::to_string(anim_time));
}

size_t AnimationNode::findRotationKeyframe(double anim_time) const {
    for (size_t i = 0; i < rotations.size() - 1; ++i) {
        if (anim_time <= rotations[i + 1].time)
            return i;
    }
	return 0;
    throw std::out_of_range("no rotation keyframe for time " + std::to_string(anim_time));
}

size_t AnimationNode::findScalingKeyframe(double anim_time) const {
    for (size_t i = 0; i < scales.size() - 1; ++i) {
        if (anim_time <= scales[i + 1].time)
            return i;
    }
    return 0;
    throw std::out_of_range("no scaling keyframe for time " + std::to_string(anim_time));
}

std::optional<glm::vec3> AnimationNode::positionLerp(double anim_time) const {
    if (positions.empty()) {
        return std::nullopt;
    }

    if (positions.size() == 1) {
        return positions[0].data;
    }

    auto index = findPositionKeyframe(anim_time);
    auto& a = positions[index];
    auto& b = positions[index + 1];

    double dt = b.time - a.time;
    double norm = (anim_time - a.time) / dt;

//    if (!(norm >= 0.f && norm <= 1.f)) {
//        throw std::runtime_error("norm >= 0.f && norm <= 1.f");
//    }

//    return glm::mix(a.data, b.data, norm);
    return a.data * (float)(1.0 - norm) + b.data * (float)norm;
}

std::optional<glm::fquat> AnimationNode::rotationLerp(double anim_time) const {
    if (rotations.empty()) {
        return std::nullopt;
    }
    if (rotations.size() == 1) {
        return rotations[0].data;
    }

    auto index = findRotationKeyframe(anim_time);
    auto& a = rotations[index];
    auto& b = rotations[index + 1];

    double dt = b.time - a.time;
    double norm = (anim_time - a.time) / dt;

//    if (!(norm >= 0.f && norm <= 1.f)) {
//        throw std::runtime_error("norm >= 0.f && norm <= 1.f");
//    }

    return glm::normalize(glm::slerp(a.data, b.data, static_cast<float>(norm)));
}

std::optional<glm::vec3> AnimationNode::scalingLerp(double anim_time) const {
    if (scales.empty()) {
        return std::nullopt;
    }

    if (scales.size() == 1) {
        return scales[0].data;
    }

    auto index = findScalingKeyframe(anim_time);
    auto a = scales[index];
    auto b = scales[index + 1];

    double dt = b.time - a.time;
    double norm = (anim_time - a.time) / dt;

//    if (!(norm >= 0.f && norm <= 1.f)) {
//        throw std::runtime_error("norm >= 0.f && norm <= 1.f");
//    }

    return glm::mix(a.data, b.data, norm);
}

SkeletalModel::SkeletalModel(
    decltype(meshes)&& meshes,
    decltype(materials)&& materials,
    decltype(bones)&& _bones,
    decltype(bone_map)&& _bone_map,
    decltype(skeletons)&& _skeletons,
    decltype(animations)&& _animations,
    std::string name
) noexcept
    : Model {std::move(meshes), std::move(materials), std::move(name)}
    , bone_map {std::move(_bone_map)}
    , animations {std::move(_animations)}
    , bones {std::move(_bones)}
    , skeletons {std::move(_skeletons)} {
}
