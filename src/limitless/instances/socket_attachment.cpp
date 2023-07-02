#include <limitless/instances/socket_attachment.hpp>

#include <limitless/instances/skeletal_instance.hpp>

using namespace Limitless;

SocketAttachment::Attachment::Attachment(std::string bone_name) noexcept
    : bone_name {std::move(bone_name)} {
}

SocketAttachment::SocketAttachment(const SocketAttachment& rhs) noexcept {
    /*
     * if InstanceAttachment gets copied, all attachments are copied too and then they have different Ids
     *
     * so we have to update data accordingly
     *
     * finding sequentially new id and appending data
     */

    uint32_t i = 0;
    const auto& skeletal_instance = static_cast<SkeletalInstance&>(*this); //NOLINT
    for (const auto& [attId, instance]: skeletal_instance.getAttachments()) {
        if (attId.type == InstanceAttachment::AttachmentType::Bone) {
            attachment_data.emplace(attId.id, std::next(rhs.attachment_data.begin(), i++)->second);
        }
    }
}

void SocketAttachment::updateSocketAttachments() {
    const auto& instance = static_cast<SkeletalInstance&>(*this); //NOLINT
    const auto& skeletal = static_cast<const SkeletalModel&>(instance.getAbstractModel()); //NOLINT
    const auto& bone_transforms = static_cast<SkeletalInstance&>(*this).getBoneTransform(); //NOLINT
    const auto& bone_map = skeletal.getBoneMap();
    const auto& bones = skeletal.getBones();

    for (auto& [_, attachment] : attachment_data) {
        auto& [bone_name, transformation_matrix] = attachment;
        const auto bone_index = bone_map.at(attachment.bone_name);
        transformation_matrix = bone_transforms.at(bone_index) * glm::inverse(bones.at(bone_index).offset_matrix);
    }

    for (const auto& [id, attachment] : instance.getAttachments()) {
        attachment->setTransformation(attachment_data.at(id.id).transformation_matrix);
    }
}

void SocketAttachment::attachToBone(std::string bone_name, std::unique_ptr<AbstractInstance> instance) {
    auto& skeletal_instance = static_cast<SkeletalInstance&>(*this); //NOLINT
    const auto& skeletal = static_cast<const SkeletalModel&>(skeletal_instance.getAbstractModel()); //NOLINT
    const auto& bone_map = skeletal.getBoneMap();

    if (bone_map.find(bone_name) == bone_map.end()) {
        throw no_such_bone("with name " + bone_name);
    }

    auto id = instance->getId();
    skeletal_instance.getAttachments().emplace(InstanceAttachment::AttachmentID {id, InstanceAttachment::AttachmentType::Bone}, std::move(instance));
    attachment_data.emplace(id, std::move(bone_name));
}

void SocketAttachment::detachFromBone(uint64_t id) {
    attachment_data.erase(id);
}
