#include <limitless/instances/abstract_instance.hpp>
#include "limitless/core/uniform/uniform_setter.hpp"

using namespace Limitless;

AbstractInstance::AbstractInstance(InstanceType _shader_type, const glm::vec3& _position) noexcept
	: id {next_id++}
	, shader_type {_shader_type}
	, position {_position} {
}

AbstractInstance::AbstractInstance(const AbstractInstance& rhs)
    : InstanceAttachment {rhs}
    , id {next_id++}
    , shader_type {rhs.shader_type}
    , final_matrix {rhs.final_matrix}
    , parent {rhs.parent}
    , transformation_matrix {rhs.transformation_matrix}
    , model_matrix {rhs.model_matrix}
    , rotation {rhs.rotation}
    , position {rhs.position}
    , scale {rhs.scale}
    , bounding_box {rhs.bounding_box}
    , shadow_cast {rhs.shadow_cast}
    , outlined {rhs.outlined}
    , hidden {rhs.hidden}
    , done {rhs.done} {
}


void AbstractInstance::updateModelMatrix() noexcept {
    const auto translation_matrix = glm::translate(glm::mat4{1.0f}, position);
    const auto rotation_matrix = glm::toMat4(rotation);
    const auto scale_matrix = glm::scale(glm::mat4{1.0f}, scale);

    model_matrix = translation_matrix * rotation_matrix * scale_matrix;
}

void AbstractInstance::updateFinalMatrix() noexcept {
	final_matrix = parent * transformation_matrix * model_matrix;
}

void AbstractInstance::reveal() noexcept {
    hidden = false;
    for (const auto& [_, attachment]: getAttachments()) {
        attachment->reveal();
    }
}

void AbstractInstance::hide() noexcept {
    hidden = true;
    for (const auto& [_, attachment]: getAttachments()) {
        attachment->hide();
    }
}

bool AbstractInstance::isHidden() const noexcept {
	return hidden;
}

void AbstractInstance::kill() noexcept {
    done = true;
}

bool AbstractInstance::isKilled() const noexcept {
    return done;
}

void AbstractInstance::castShadow() noexcept {
	shadow_cast = true;
    for (const auto& [_, attachment]: getAttachments()) {
        attachment->castShadow();
    }
}

void AbstractInstance::removeShadow() noexcept {
	shadow_cast = false;
    for (const auto& [_, attachment]: getAttachments()) {
        attachment->removeShadow();
    }
}

bool AbstractInstance::doesCastShadow() const noexcept {
	return shadow_cast;
}

AbstractInstance& AbstractInstance::setPosition(const glm::vec3& _position) noexcept {
    position = _position;
    return *this;
}

AbstractInstance& AbstractInstance::setRotation(const glm::quat& _rotation) noexcept {
    rotation = _rotation;
    return *this;
}

AbstractInstance& AbstractInstance::rotateBy(const glm::quat& _rotation) noexcept {
    rotation = _rotation * rotation;
    return *this;
}

AbstractInstance& AbstractInstance::setScale(const glm::vec3& _scale) noexcept {
    scale = _scale;
    return *this;
}

AbstractInstance& AbstractInstance::setTransformation(const glm::mat4& transformation) {
	transformation_matrix = transformation;
	return *this;
}

AbstractInstance& AbstractInstance::setParent(const glm::mat4& _parent) noexcept {
	parent = _parent;
	return *this;
}

void AbstractInstance::draw(Context& ctx, const Assets& assets, ShaderType material_shader_type, ms::Blending blending) {
    draw(ctx, assets, material_shader_type, blending, UniformSetter {});
}

void AbstractInstance::update(Context& context, const Camera& camera) {
	// updates current model matrices
	updateModelMatrix();
	updateFinalMatrix();
	updateBoundingBox();

	// propagates current instance values to attachments
    InstanceAttachment::setAttachmentsParent(final_matrix);
    InstanceAttachment::updateAttachments(context, camera);
}

void AbstractInstance::removeOutline() noexcept {
	outlined = false;
    for (const auto& [_, attachment]: getAttachments()) {
        attachment->removeOutline();
    }
}

void AbstractInstance::makeOutlined() noexcept {
	outlined = true;
    for (const auto& [_, attachment]: getAttachments()) {
        attachment->makeOutlined();
    }
}

bool AbstractInstance::isOutlined() const noexcept {
	return outlined;
}
