#include <limitless/instances/abstract_instance.hpp>
#include <limitless/core/uniform_setter.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <limitless/postprocessing.hpp>

using namespace LimitlessEngine;

void AbstractInstance::calculateModelMatrix() noexcept {
    const auto translation_matrix = glm::translate(glm::mat4{1.0f}, position);

    const auto rotation_matrix = glm::toMat4(rotation);

    const auto scale_matrix = glm::scale(glm::mat4{1.0f}, scale);

    model_matrix = translation_matrix * rotation_matrix * scale_matrix;
}

void AbstractInstance::reveal() noexcept {
    hidden = false;
}

void AbstractInstance::hide() noexcept {
    hidden = true;
}

void AbstractInstance::kill() noexcept {
    done = true;
}

bool AbstractInstance::isKilled() const noexcept {
    return done;
}

AbstractInstance& AbstractInstance::setPosition(const glm::vec3& _position) noexcept {
    EffectAttachable::setPosition(_position);
    LightAttachable::setPosition(_position);

    position = _position;
    return *this;
}

AbstractInstance& AbstractInstance::setRotation(const glm::quat& _rotation) noexcept {
    EffectAttachable::setRotation(_rotation);
    LightAttachable::setRotation(_rotation);

    rotation = _rotation;
    return *this;
}

AbstractInstance& AbstractInstance::rotateBy(const glm::quat& _rotation) noexcept {
    EffectAttachable::rotateBy(_rotation);
    LightAttachable::rotateBy(_rotation);

    rotation = _rotation * rotation;
    return *this;
}


AbstractInstance& AbstractInstance::setScale(const glm::vec3& _scale) noexcept {
    scale = _scale;
    return *this;
}

void AbstractInstance::draw(Context& ctx, const Assets& assets, MaterialShader material_shader_type, Blending blending) {
    draw(ctx, assets, material_shader_type,blending, UniformSetter{});
}

void AbstractInstance::update() {
    EffectAttachable::update();

    calculateModelMatrix();
    calculateBoundingBox();
}

AbstractInstance::AbstractInstance(Lighting* _lighting, ModelShader _shader_type, const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale) noexcept
    : EffectAttachable{}
    , LightAttachable{_lighting}
    , id{next_id++}
    , shader_type{_shader_type}
    , position{_position}
    , rotation{_rotation}
    , scale{_scale} {
}

AbstractInstance::AbstractInstance(ModelShader _shader_type, const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale) noexcept
    : EffectAttachable{}
    , id{next_id++}
    , shader_type{_shader_type}
    , position{_position}
    , rotation{_rotation}
    , scale{_scale} {
}