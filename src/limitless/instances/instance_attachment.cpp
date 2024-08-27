#include <limitless/instances/instance_attachment.hpp>
#include <limitless/instances/instance.hpp>

#include <stdexcept>
#include <algorithm>

using namespace Limitless;

bool InstanceAttachment::AttachmentID::operator<(const Limitless::InstanceAttachment::AttachmentID& rhs) const {
    return std::tie(id, type) < std::tie(rhs.id, rhs.type);
}

InstanceAttachment::InstanceAttachment(const InstanceAttachment& attachment) {
	for (const auto& [attId, att] : attachment.attachments) {
        auto clone = att->clone();
        auto id = clone->getId();
		attachments.emplace(AttachmentID {id, attId.type}, std::move(clone));
	}
}

void InstanceAttachment::updateAttachments(const Camera& camera) {
	for (const auto& [_, attachment] : attachments) {
        attachment->update(camera);
	}
}

InstanceAttachment& InstanceAttachment::setAttachmentsParent(const glm::mat4& parent) noexcept {
	for (const auto& [_, attachment] : attachments) {
		attachment->setParent(parent);
	}
	return *this;
}

void InstanceAttachment::attach(std::shared_ptr<Instance> attachment) {
	attachments.emplace(AttachmentID {attachment->getId(), AttachmentType::Basic}, std::move(attachment));
}

void InstanceAttachment::detach(uint64_t id) {
    auto it = std::find_if(attachments.begin(), attachments.end(), [&] (const auto& att) {
        return att.first.id == id;
    });

    if (it != attachments.end()) {
        attachments.erase(it);
    }
}

const Instance& InstanceAttachment::getAttachment(uint64_t id) const {
    auto it = std::find_if(attachments.begin(), attachments.end(), [&] (const auto& att) {
        return att.first.id == id;
    });

    if (it != attachments.end()) {
        return *it->second;
    } else {
        throw no_such_attachment("id " + std::to_string(id));
    }
}

Instance& InstanceAttachment::getAttachment(uint64_t id) {
    auto it = std::find_if(attachments.begin(), attachments.end(), [&] (const auto& att) {
        return att.first.id == id;
    });

    if (it != attachments.end()) {
        return *it->second;
    } else {
        throw no_such_attachment("id " + std::to_string(id));
    }
}
