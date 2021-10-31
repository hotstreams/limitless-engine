#include <limitless/instances/abstract_instance.hpp>
#include <limitless/core/uniform_setter.hpp>

using namespace Limitless;

AbstractInstance::AbstractInstance(ModelShader _shader_type, const glm::vec3& _position) noexcept
	: id {next_id++}
	, shader_type {_shader_type}
	, position {_position} {
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
}

void AbstractInstance::hide() noexcept {
    hidden = true;
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
}

void AbstractInstance::removeShadow() noexcept {
	shadow_cast = false;
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

AbstractInstance& AbstractInstance::setParent(const glm::mat4& _parent) {
	parent = _parent;
	return *this;
}

void AbstractInstance::draw(Context& ctx, const Assets& assets, ShaderPass material_shader_type, ms::Blending blending) {
    draw(ctx, assets, material_shader_type, blending, UniformSetter {});
}

void AbstractInstance::updateAttachments(Context& context, const Camera& camera) {
	InstanceAttachment::setParent(final_matrix);
	InstanceAttachment::updateAttachments(context, camera);
}

void AbstractInstance::update(Context& context, const Camera& camera) {
	// updates current model matrices
	updateModelMatrix();
	updateFinalMatrix();
	updateBoundingBox();

	// propagates current instance values to attachments
	updateAttachments(context, camera);
}

void AbstractInstance::removeOutline() noexcept {
	outlined = false;
}

void AbstractInstance::makeOutlined() noexcept {
	outlined = true;
}

bool AbstractInstance::isOutlined() const noexcept {
	return outlined;
}
