#include <limitless/instances/instance_attachment.hpp>
#include <limitless/instances/abstract_instance.hpp>
#include <stdexcept>

using namespace Limitless;

InstanceAttachment::InstanceAttachment(const InstanceAttachment& attachment) {
	for (const auto& [id, att] : attachment.attachments) {
		attachments.emplace(id, att->clone());
	}
}

void InstanceAttachment::updateAttachments(Context& context, const Camera& camera) {
	for (const auto& [_, attachment] : attachments) {
        attachment->update(context, camera);
	}
}

InstanceAttachment& InstanceAttachment::setParent(const glm::mat4& parent) noexcept {
	for (const auto& [_, attachment] : attachments) {
		attachment->setParent(parent);
	}
	return *this;
}

void InstanceAttachment::attach(std::unique_ptr<AbstractInstance> attachment) {
	const auto [it, success] = attachments.emplace(attachment->getId(), std::move(attachment));
	if (!success) {
		throw std::logic_error {"Attachment already attached! id: " + std::to_string(attachment->getId())};
	}
}

void InstanceAttachment::detach(uint64_t id) {
	const auto erased = attachments.erase(id);
	if (erased == 0) {
		throw std::logic_error {"There is no such attachment! id: " + std::to_string(id)};
	}
}

const std::unique_ptr<AbstractInstance>& InstanceAttachment::getAttachment(uint64_t id) const {
	return attachments.at(id);
}

std::unique_ptr<AbstractInstance>& InstanceAttachment::getAttachment(uint64_t id) {
	return attachments.at(id);
}