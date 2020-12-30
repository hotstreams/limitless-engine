#include <skeletal_instance.hpp>
#include <shader_types.hpp>
#include <shader_storage.hpp>
#include <iostream>

using namespace GraphicsEngine;

constexpr auto skeletal_buffer_name = "bone_buffer";

SkeletalInstance::SkeletalInstance(std::shared_ptr<AbstractModel> m, const glm::vec3& position)
    : ModelInstance{std::move(m), position} {
    shader_type = ModelShader::Skeletal;
    auto& skeletal = static_cast<SkeletalModel&>(*model);

    bone_transform.resize(skeletal.getBones().size(), glm::mat4(1.0f));
    bone_buffer = BufferBuilder::buildIndexed(skeletal_buffer_name, Buffer::Type::ShaderStorage, bone_transform, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);
}

SkeletalInstance::SkeletalInstance(Lighting *lighting, std::shared_ptr<AbstractModel> m, const glm::vec3& position)
    : ModelInstance{lighting, std::move(m), position} {
    shader_type = ModelShader::Skeletal;
    auto& skeletal = static_cast<SkeletalModel&>(*model);

    bone_transform.resize(skeletal.getBones().size(), glm::mat4(1.0f));
    bone_buffer = BufferBuilder::buildIndexed(skeletal_buffer_name, Buffer::Type::ShaderStorage, bone_transform, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);
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

void SkeletalInstance::draw(MaterialShader material_shader, Blending blending, const UniformSetter& uniform_setter) {
    if (hidden) {
        return;
    }

    calculateModelMatrix();

    for (auto& [name, mesh] : meshes) {
        if (mesh.isHidden()) {
            continue;
        }

        // iterates over all material layers
        auto first_opaque {true};
        for (const auto& [layer, material] : mesh.getMaterial()) {
            // following blending order
            if (material->getBlending() == blending) {
                // sets blending mode dependent on layers count
                if (blending == Blending::Opaque && mesh.getMaterial().count() > 1 && !first_opaque) {
                    setBlendingMode(Blending::OpaqueHalf);
                } else {
                    setBlendingMode(blending);
                    if (blending == Blending::Opaque) first_opaque = false;
                }
                // gets required shader from storage
                auto& shader = *shader_storage.get(material_shader, shader_type, material->getShaderIndex());

                // updates model/material uniforms
                shader << *material
                       << UniformValue{"model", model_matrix};

                // sets custom pass-dependent uniforms
                for (const auto& uniform_set : uniform_setter) {
                    uniform_set(shader);
                }

                bone_buffer->bindBase(IndexedBuffer::getBindingPoint(IndexedBuffer::Type::ShaderStorage, skeletal_buffer_name));

                shader.use();

                mesh.draw();
            }
        }
    }

    bone_buffer->fence();
}

void SkeletalInstance::play(const std::string& name) {
    const auto& skeletal = static_cast<SkeletalModel&>(*model);

    const auto& animations = skeletal.getAnimations();

    const auto found = std::find_if(animations.begin(), animations.end(), [&] (const auto& anim) { return name == anim.name; });
    if (found == animations.end()) {
        throw std::runtime_error("Animation not found " + name);
    } else {
        animation = &(*found);
    }
}

void SkeletalInstance::pause() noexcept {
    paused = true;
}

void SkeletalInstance::resume() noexcept {
    paused = false;
}

void SkeletalInstance::update() {
    AbstractInstance::update();

    if (animation == nullptr || paused) {
        return;
    }

    auto& skeletal = static_cast<SkeletalModel&>(*model);

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

	    for (size_t i = 0; i < node.size(); ++i) {
            node_traversal(node[i], transform);
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

SkeletalInstance *SkeletalInstance::clone() noexcept {
    return new SkeletalInstance(*this);
}
