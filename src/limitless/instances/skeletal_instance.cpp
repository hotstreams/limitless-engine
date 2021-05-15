#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/pipeline/shader_pass_types.hpp>
#include <limitless/shader_storage.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/core/context.hpp>
#include <limitless/assets.hpp>
#include <limitless/ms/material.hpp>

using namespace Limitless;

constexpr auto skeletal_buffer_name = "bone_buffer";

void SkeletalInstance::initializeBuffer() {
    BufferBuilder builder;
    bone_buffer = builder.setTarget(Buffer::Type::ShaderStorage)
            .setUsage(Buffer::Usage::DynamicDraw)
            .setAccess(Buffer::MutableAccess::WriteOrphaning)
            .setData(bone_transform.data())
            .setDataSize(bone_transform.size() * sizeof(glm::mat4))
            .build();
}

SkeletalInstance::SkeletalInstance(std::shared_ptr<AbstractModel> m, const glm::vec3& position)
    : ModelInstance(ModelShader::Skeletal, std::move(m), position) {
    auto& skeletal = dynamic_cast<SkeletalModel&>(*model);

    bone_transform.resize(skeletal.getBones().size(), glm::mat4(1.0f));
    initializeBuffer();
}

SkeletalInstance::SkeletalInstance(Lighting *lighting, std::shared_ptr<AbstractModel> m, const glm::vec3& position)
    : ModelInstance(ModelShader::Skeletal, lighting, std::move(m), position) {
    auto& skeletal = dynamic_cast<SkeletalModel&>(*model);

    bone_transform.resize(skeletal.getBones().size(), glm::mat4(1.0f));
    initializeBuffer();
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

SkeletalInstance& SkeletalInstance::setPosition(const glm::vec3& position) noexcept {
    AbstractInstance::setPosition(position);
    return *this;
}

SkeletalInstance& SkeletalInstance::setRotation(const glm::quat& rotation) noexcept {
    AbstractInstance::setRotation(rotation);
    return *this;
}

SkeletalInstance& SkeletalInstance::setScale(const glm::vec3& scale) noexcept {
    AbstractInstance::setScale(scale);
    return *this;
}

void SkeletalInstance::draw(Context& ctx, const Assets& assets, ShaderPass pass, ms::Blending blending, const UniformSetter& uniform_setter) {
    if (hidden) {
        return;
    }

    calculateModelMatrix();

    bone_buffer->bindBase(ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, skeletal_buffer_name));

    // iterates over all meshes
    for (auto& [name, mesh] : meshes) {
        mesh.draw(ctx, assets, pass, shader_type, model_matrix, blending, uniform_setter);
    }

    bone_buffer->fence();
}

SkeletalInstance& SkeletalInstance::play(const std::string& name) {
    const auto& skeletal = dynamic_cast<SkeletalModel&>(*model);
    const auto& animations = skeletal.getAnimations();

    const auto found = std::find_if(animations.begin(), animations.end(), [&] (const auto& anim) { return name == anim.name; });
    if (found == animations.end()) {
        throw std::runtime_error("Animation not found " + name);
    } else {
        animation = &(*found);
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

void SkeletalInstance::update(Context& context, Camera& camera) {
    AbstractInstance::update(context, camera);

    if (!animation || paused) {
        return;
    }

    auto& skeletal = dynamic_cast<SkeletalModel&>(*model);

    auto& bones = skeletal.getBones();

    const Animation& anim = *animation;

    double anim_time = fmod(glfwGetTime() * anim.tps, anim.duration);

    std::function<void(const Tree<uint32_t>&, const glm::mat4&)> node_traversal;

    node_traversal = [&](const Tree<uint32_t>& node, const glm::mat4& parent_mat) {
        auto anim_node = findAnimationNode(bones[*node]);

        auto local_transform = !bones[*node].isFake() ? bones[*node].node_transform : glm::mat4(1.f);

	    if (anim_node) {
            glm::vec3 scale = anim_node->scalingLerp(anim_time);
            glm::vec3 position = anim_node->positionLerp(anim_time);
            auto rotation = anim_node->rotationLerp(anim_time);

            auto translate = glm::translate(glm::mat4(1.f), position);
            auto rotate = glm::mat4_cast(rotation);
            auto scale_mat = glm::scale(glm::mat4(1.f), scale);

            local_transform = translate * rotate * scale_mat;
        }

        auto transform = parent_mat * local_transform;
	    bone_transform[*node] = skeletal.getGlobalInverseMatrix() * transform * bones[*node].offset_matrix;

	    for (const auto& n : node) {
            node_traversal(n, transform);
        }
    };

    try {
        node_traversal(skeletal.getSkeletonTree(), glm::mat4(1.f));
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Wrong TPS/duration. " + std::string(e.what()));
    }

    bone_buffer->mapData(bone_transform.data(), sizeof(glm::mat4) * bone_transform.size());
}

SkeletalInstance* SkeletalInstance::clone() noexcept {
    return new SkeletalInstance(*this);
}

void SkeletalInstance::calculateBoundingBox() noexcept {
    ModelInstance::calculateBoundingBox();
}
