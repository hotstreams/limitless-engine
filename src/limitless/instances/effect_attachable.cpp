#include <limitless/instances/effect_attachable.hpp>
#include <limitless/instances/effect_instance.hpp>
#include <limitless/fx/emitters/emitter.hpp>

using namespace Limitless;

EffectAttachable::Attachment::Attachment(const glm::vec3& _offset, const std::shared_ptr<EffectInstance>& effect, const glm::vec3& position) noexcept
    : instance {std::make_unique<EffectInstance>(effect, position)}
    , offset {_offset} {
    instance->setPosition(position + offset);
}

EffectAttachable::Attachment::Attachment(const glm::vec3& _offset, Lighting* lighting, const std::shared_ptr<EffectInstance>& effect, const glm::vec3& position) noexcept
    : instance{std::make_unique<EffectInstance>(lighting, effect, position)}
    , offset{_offset} {
    instance->setPosition(position + offset);
}

EffectAttachable::Attachment& EffectAttachable::operator[](uint64_t index) noexcept {
    return attachments[index];
}

EffectAttachable::Attachment& EffectAttachable::at(uint64_t index) {
    return attachments.at(index);
}

EffectAttachable::Attachment::Attachment(const Attachment& attachment)
    : instance {attachment.instance->clone()}
    , offset {attachment.offset} {
}

void EffectAttachable::detachEffect(uint64_t index) noexcept {
    attachments.erase(attachments.begin() + index);
}

EffectAttachable::EffectAttachable() noexcept = default;
EffectAttachable::~EffectAttachable() = default;

EffectAttachable::EffectAttachable(const EffectAttachable&) = default;
EffectAttachable::EffectAttachable(EffectAttachable&&) noexcept = default;

void EffectAttachable::update(Context& context, Camera& camera) {
    for (auto& [instance, offset] : attachments) {
        instance->update(context, camera);
    }
}

void EffectAttachable::setPosition(const glm::vec3& position) noexcept {
    for (auto& [instance, offset] : attachments)
        instance->setPosition(position + offset);
}

void EffectAttachable::setRotation(const glm::quat& rotation) noexcept {
    for (auto& [instance, offset] : attachments)
        instance->setRotation(rotation);
}

void EffectAttachable::rotateBy(const glm::quat& rotation) noexcept {
    for (auto& [instance, offset] : attachments) {
        instance->rotateBy(rotation);
    }
}

