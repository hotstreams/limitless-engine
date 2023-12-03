#include <limitless/instances/instance_builder.hpp>
#include <limitless/models/skeletal_model.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/models/elementary_model.hpp>
#include <limitless/instances/decal_instance.hpp>

using namespace Limitless;

void Instance::Builder::initialize(const std::shared_ptr<ModelInstance>& instance) {
    // apply properties
    instance->setPosition(position_);
    instance->setRotation(rotation_);
    instance->setScale(scale_);
    if (cast_shadow_) {
        instance->castShadow();
    }

    // add instance attachments
    for (const auto &attachment: attachments) {
        instance->attach(attachment);
    }

    // add bone attachments
    if (instance->getInstanceType() == InstanceType::Skeletal) {
        auto &skeletal = static_cast<SkeletalInstance &>(*instance);

        for (const auto &[bone, attachment]: bone_attachments) {
            skeletal.attachToBone(bone, attachment);
        }
    }

    // change materials
    if (global_material) {
        instance->changeMaterials(global_material);
    }

    for (const auto& [index, name, material] : changed_materials) {
        if (!name.empty()) {
            instance->changeMaterial(name, material);
        } else {
            instance->changeMaterial(index, material);
        }
    }
}

Instance::Builder& Instance::Builder::model(const std::shared_ptr<AbstractModel>& model) {
    model_ = model;
    return *this;
}

Instance::Builder &Instance::Builder::position(const glm::vec3& position) {
    position_ = position;
    return *this;
}

Instance::Builder &Instance::Builder::rotation(const glm::quat& rotation) {
    rotation_ = rotation;
    return *this;
}

Instance::Builder &Instance::Builder::scale(const glm::vec3& scale) {
    scale_ = scale;
    return *this;
}

Instance::Builder &Instance::Builder::cast_shadow(bool cast_shadow) {
    cast_shadow_ = cast_shadow;
    return *this;
}

std::shared_ptr<ModelInstance> Instance::Builder::asModel() {
    if (dynamic_cast<Model*>(model_.get())) {
        auto instance = std::make_shared<ModelInstance>(model_, position_);
        initialize(instance);
        return instance;
    }

    if (dynamic_cast<ElementaryModel*>(model_.get())) {
        if (global_material) {
            auto instance = std::make_shared<ModelInstance>(model_, global_material, position_);
            initialize(instance);
            return instance;
        } else {
            throw instance_builder_exception {"Material for Elementary model is not set!"};
        }
    }

    throw instance_builder_exception {"Invalid parameters for instance builder!"};
}

std::shared_ptr<SkeletalInstance> Instance::Builder::asSkeletal() {
    if (dynamic_cast<SkeletalModel*>(model_.get())) {
        auto instance = std::make_shared<SkeletalInstance>(model_, position_);
        initialize(instance);
        return instance;
    } else {
        throw instance_builder_exception {model_->getName() + " is not SkeletalModel!"};
    }
}

std::shared_ptr<Instance> Instance::Builder::build() {
    if (model_) {
        if (dynamic_cast<SkeletalModel*>(model_.get())) {
            return asSkeletal();
        }

        if (dynamic_cast<Model*>(model_.get())) {
            return asModel();
        }

        if (dynamic_cast<ElementaryModel *>(model_.get())) {
            if (global_material) {
                auto instance = std::make_shared<ModelInstance>(model_, global_material, position_);
                initialize(instance);
                return instance;
            } else {
                throw instance_builder_exception {"Material for Elementary model is not set!"};
            }
        }

        throw instance_builder_exception {"Invalid parameters for instance builder!"};
    }

    if (effect_) {
        auto instance = std::make_shared<EffectInstance>(effect_, position_);
        instance->setPosition(position_);
        instance->setRotation(rotation_);
        instance->setScale(scale_);
        return instance;
    }

    throw instance_builder_exception {"Invalid parameters for instance builder!"};
}

Instance::Builder& Instance::Builder::material(uint32_t mesh_index, const std::shared_ptr<ms::Material>& material) {
    changed_materials.emplace_back(MaterialChange{mesh_index, {}, material});
    return *this;
}

Instance::Builder& Instance::Builder::material(const std::string& mesh_name, const std::shared_ptr<ms::Material>& material) {
    changed_materials.emplace_back(MaterialChange{0, mesh_name, material});
    return *this;
}


Instance::Builder& Instance::Builder::material(const std::shared_ptr<ms::Material>& material) {
    global_material = material;
    return *this;
}

Instance::Builder& Instance::Builder::attach(const std::shared_ptr<Instance>& instance) {
    attachments.emplace_back(instance);
    return *this;
}

Instance::Builder& Instance::Builder::attach(const std::string& bone_name, const std::shared_ptr<Instance>& instance) {
    bone_attachments.emplace_back(SocketAttachment{bone_name, instance});
    return *this;
}

Instance::Builder& Instance::Builder::effect(const std::shared_ptr<EffectInstance>& effect) {
    effect_ = effect;
    return *this;
}

std::shared_ptr<EffectInstance> Instance::Builder::asEffect() {
    if (effect_) {
        auto instance = std::make_shared<EffectInstance>(effect_, position_);
        instance->setPosition(position_);
        instance->setRotation(rotation_);
        instance->setScale(scale_);
        return instance;
    } else {
        throw instance_builder_exception {"Instance builder does not have effect to build!"};
    }
}

std::shared_ptr<DecalInstance> Instance::Builder::asDecal() {
    if (!global_material) {
        throw instance_builder_exception {"Material for decal is not set!"};
    }

    if (dynamic_cast<ElementaryModel*>(model_.get())) {
        throw instance_builder_exception {"Model for decal is not elementary!"};
    }

    return std::make_shared<DecalInstance>(model_, global_material, position_);
}
