#include <limitless/skeletal_model.hpp>

using namespace LimitlessEngine;

AnimationNode::AnimationNode(decltype(positions) positions, decltype(rotations) rotations, decltype(scales) scales, Bone& bone) noexcept
    : positions(std::move(positions)), rotations(std::move(rotations)), scales(std::move(scales)), bone(bone) {}

size_t AnimationNode::findPositionKeyframe(double anim_time) const {
    for (size_t i = 0; i < positions.size() - 1; ++i) {
        if (anim_time < positions[i + 1].time) {
            return i;
        }
    }
    throw std::out_of_range("no position keyframe for time " + std::to_string(anim_time));
}

size_t AnimationNode::findRotationKeyframe(double anim_time) const {
    for (size_t i = 0; i < rotations.size() - 1; ++i) {
        if (anim_time < rotations[i + 1].time)
            return i;
    }
    throw std::out_of_range("no rotation keyframe for time " + std::to_string(anim_time));
}

size_t AnimationNode::findScalingKeyframe(double anim_time) const {
    for (size_t i = 0; i < scales.size() - 1; ++i) {
        if (anim_time < scales[i + 1].time)
            return i;
    }
    throw std::out_of_range("no scaling keyframe for time " + std::to_string(anim_time));
}

glm::vec3 AnimationNode::positionLerp(double anim_time) const {
    if (positions.empty()) {
        return glm::vec3{0.0f};
    }

    if (positions.size() == 1) {
        return positions[0].data;
    }

    auto index = findPositionKeyframe(anim_time);
    auto& a = positions[index];
    auto& b = positions[index + 1];

    double dt = b.time - a.time;
    double norm = (anim_time - a.time) / dt;

    if (!(norm >= 0.f && norm <= 1.f)) {
        throw std::runtime_error("norm >= 0.f && norm <= 1.f");
    }

    return a.data + (b.data - a.data) * static_cast<float>(norm);
}

glm::fquat AnimationNode::rotationLerp(double anim_time) const {
    if (rotations.empty()) {
        return glm::fquat{1.f, 0.f, 0.f, 0.f};
    }
    if (rotations.size() == 1) {
        return rotations[0].data;
    }

    auto index = findRotationKeyframe(anim_time);
    auto& a = rotations[index];
    auto& b = rotations[index + 1];

    double dt = b.time - a.time;
    double norm = (anim_time - a.time) / dt;

    if (!(norm >= 0.f && norm <= 1.f)) {
        throw std::runtime_error("norm >= 0.f && norm <= 1.f");
    }

    return glm::normalize(glm::lerp(a.data, b.data, static_cast<float>(norm)));
}

glm::vec3 AnimationNode::scalingLerp(double anim_time) const {
    if (scales.empty()) {
        return glm::vec3{1.f};
    }

    if (scales.size() == 1) {
        return scales[0].data;
    }

    auto index = findScalingKeyframe(anim_time);
    auto a = scales[index];
    auto b = scales[index + 1];

    double dt = b.time - a.time;
    double norm = (anim_time - a.time) / dt;

    if (!(norm >= 0.f && norm <= 1.f)) {
        throw std::runtime_error("norm >= 0.f && norm <= 1.f");
    }

    return a.data + (b.data - a.data) * static_cast<float>(norm);
}

SkeletalModel::SkeletalModel(decltype(meshes)&& meshes, decltype(materials)&& materials, decltype(bones)&& bones, decltype(bone_map)&& bone_map, decltype(skeleton)&& skeleton, decltype(animations)&& animations, const glm::mat4& global_matrix) noexcept
    : Model{std::move(meshes), std::move(materials)}, bones{std::move(bones)}, bone_map{std::move(bone_map)},  animations{std::move(animations)}, skeleton{std::move(skeleton)}, global_inverse{global_matrix} {

}
