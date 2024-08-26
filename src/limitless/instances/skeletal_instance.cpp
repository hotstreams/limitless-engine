#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/shader_storage.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include <limitless/core/context.hpp>
#include <limitless/assets.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/vertex.hpp>
#include <limitless/models/mesh.hpp>
#include <limitless/core/skeletal_stream.hpp>
#include <iostream>

using namespace Limitless;

constexpr auto SKELETAL_BUFFER_NAME = "bone_buffer";

void SkeletalInstance::initializeBuffer() {
    bone_buffer = Buffer::builder()
            .target(Buffer::Type::ShaderStorage)
            .usage(Buffer::Usage::DynamicDraw)
            .access(Buffer::MutableAccess::WriteOrphaning)
            .data(bone_transform.data())
            .size(bone_transform.size() * sizeof(glm::mat4))
            .build();
}

void SkeletalInstance::updateAnimationFrame() {
    if (!animation || paused) {
        return;
    }

    auto& skeletal = dynamic_cast<SkeletalModel&>(*model);
    auto& bones = skeletal.getBones();
    const Animation& anim = *animation;

    const auto current_time = std::chrono::steady_clock::now();
    if (last_time == std::chrono::time_point<std::chrono::steady_clock>()) {
        last_time = current_time;
    }
    const auto delta_time = current_time - last_time;
    animation_duration += delta_time;
    last_time = current_time;
    const auto animation_time = glm::mod(animation_duration.count() * anim.tps, anim.duration);

    std::function<void(const Tree<uint32_t>&, const glm::mat4&)> node_traversal;
    node_traversal = [&](const Tree<uint32_t>& node, const glm::mat4& parent_mat) {
        const auto bone_indice = *node;
        auto& bone = bones[bone_indice];

        auto local_transform = [&]() -> glm::mat4 {
            auto* anim_node = findAnimationNode(bone);
            if (!anim_node) {
                return bone.node_transform;
            }

            auto maybe_anim_position = anim_node->positionLerp(animation_time);
            auto maybe_anim_rotation = anim_node->rotationLerp(animation_time);
            auto maybe_anim_scale = anim_node->scalingLerp(animation_time);

            auto position = maybe_anim_position? *maybe_anim_position : bone.position;
            auto rotation = maybe_anim_rotation? *maybe_anim_rotation : bone.rotation;
            auto scale = maybe_anim_scale? *maybe_anim_scale : bone.scale;

            auto translate = glm::translate(glm::mat4(1.f), position);
            auto rotate = glm::mat4_cast(rotation);
            auto scale_mat = glm::scale(glm::mat4(1.f), scale);

            return translate * rotate * scale_mat;
        }();

        auto global_transform = parent_mat * local_transform;

        if (bone.joint_index) {
            bone_transform[*bone.joint_index] = global_transform * bone.offset_matrix;
        }

        for (const auto& n : node) {
            node_traversal(n, global_transform);
        }
    };

    try {
        for (auto& hierarchy : skeletal.getSkeletonTrees()) {
            node_traversal(hierarchy, glm::mat4(1.f));
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Wrong TPS/duration. " + std::string(e.what()));
    }

    bone_buffer->mapData(bone_transform.data(), sizeof(glm::mat4) * bone_transform.size());
}

const AnimationNode* SkeletalInstance::findAnimationNode(const Bone& bone) const noexcept {
    if (animation) {
        for (auto& node : animation->nodes) {
            if (&node.bone == &bone) {
                return &node;
            }
        }
    }

    return nullptr;
}

SkeletalInstance::SkeletalInstance(std::shared_ptr<AbstractModel> m, const glm::vec3& position)
    : ModelInstance(InstanceType::Skeletal, std::move(m), position) {
    //TODO: check type
    auto& skeletal = dynamic_cast<SkeletalModel&>(*model);
    auto& bones = skeletal.getBones();

    size_t skinned_bones = std::count_if(bones.begin(), bones.end(), [](const auto& bone) {
        return bone.joint_index.has_value();
    });

    bone_transform.resize(skinned_bones, glm::mat4(1.0f));
    initializeBuffer();
}

SkeletalInstance::SkeletalInstance(const SkeletalInstance& rhs) noexcept
    : ModelInstance {rhs}
    , SocketAttachment {rhs}
    , bone_transform {rhs.bone_transform}
    , animation {rhs.animation}
    , paused {rhs.paused}
    , last_time {rhs.last_time}
    , animation_duration {rhs.animation_duration} {
    initializeBuffer();
}


std::unique_ptr<Instance> SkeletalInstance::clone() noexcept {
    return std::make_unique<SkeletalInstance>(*this);
}

void SkeletalInstance::update(Context& context, const Camera& camera) {
    updateAnimationFrame();

    SocketAttachment::updateSocketAttachments();

    ModelInstance::update(context, camera);
}

SkeletalInstance &SkeletalInstance::play(uint32_t index) {
    const auto& skeletal = dynamic_cast<SkeletalModel&>(*model);
    const auto& animations = skeletal.getAnimations();

    animation = &animations.at(index);
    animation_duration = std::chrono::seconds(0);
    last_time = std::chrono::time_point<std::chrono::steady_clock>();

    return *this;
}

SkeletalInstance& SkeletalInstance::play(const std::string& name) {
    const auto& skeletal = dynamic_cast<SkeletalModel&>(*model);
    const auto& animations = skeletal.getAnimations();

    const auto found = std::find_if(animations.begin(), animations.end(), [&] (const auto& anim) { return name == anim.name; });
    if (found == animations.end()) {
        throw no_such_animation("with name " + name);
    } else {
        animation = &(*found);
        animation_duration = std::chrono::seconds(0);
        last_time = std::chrono::time_point<std::chrono::steady_clock>();
    }

    return *this;
}

SkeletalInstance& SkeletalInstance::pause() noexcept {
    paused = true;
    return *this;
}

SkeletalInstance& SkeletalInstance::resume() noexcept {
    paused = false;
    return *this;
}

SkeletalInstance& SkeletalInstance::stop() noexcept {
    animation = nullptr;
    return *this;
}

glm::vec3 SkeletalInstance::getSkinnedVertexPosition(const std::shared_ptr<AbstractMesh>& mesh, size_t vertex_index) const {
    const auto& skinned_mesh = dynamic_cast<SkinnedVertexStream<VertexNormalTangent>&>(dynamic_cast<Mesh&>(*mesh).getVertexStream());

    const auto& bone_weight = skinned_mesh.getBoneWeights().at(vertex_index);
    const auto& vertex = skinned_mesh.getVertices().at(vertex_index);

    auto transform = bone_transform[bone_weight.bone_index[0]] * bone_weight.weight[0];
    transform     += bone_transform[bone_weight.bone_index[1]] * bone_weight.weight[1];
    transform     += bone_transform[bone_weight.bone_index[2]] * bone_weight.weight[2];
    transform     += bone_transform[bone_weight.bone_index[3]] * bone_weight.weight[3];

    auto matrix = final_matrix;
    matrix *= transform;

    return matrix * glm::vec4(vertex.position, 1.0);
}

const std::vector<Animation>& SkeletalInstance::getAllAnimations() const noexcept {
    const auto& skeletal = dynamic_cast<SkeletalModel&>(*model);
    const auto& animations = skeletal.getAnimations();
    return animations;
}

const std::vector<Bone>& SkeletalInstance::getAllBones() const noexcept {
    const auto& skeletal = dynamic_cast<SkeletalModel&>(*model);
    const auto& bones = skeletal.getBones();
    return bones;
}
