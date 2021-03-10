#include <abstract_instance.hpp>

#include <glm/gtc/matrix_transform.hpp>

using namespace LimitlessEngine;

void AbstractInstance::calculateModelMatrix() noexcept {
    model_matrix = glm::mat4{1.0f};

    model_matrix = glm::translate(model_matrix, position);

    model_matrix = glm::rotate(model_matrix, rotation.x, glm::vec3{1.0f, 0.f, 0.f});
    model_matrix = glm::rotate(model_matrix, rotation.y, glm::vec3{0.0f, 1.f, 0.f});
    model_matrix = glm::rotate(model_matrix, rotation.z, glm::vec3{0.0f, 0.f, 1.f});

    model_matrix = glm::scale(model_matrix, scale);
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

void AbstractInstance::asWireFrame() noexcept {
    wireframe = true;
}

void AbstractInstance::asModel() noexcept {
    wireframe = false;
}

AbstractInstance& AbstractInstance::setPosition(const glm::vec3& _position) noexcept {
    EffectAttachable::setPosition(_position);
    LightAttachable::setPosition(_position);

    position = _position;
    return *this;
}

AbstractInstance& AbstractInstance::setRotation(const glm::vec3& _rotation) noexcept {
    EffectAttachable::setRotation(_rotation);
    LightAttachable::setRotation(_rotation);

    rotation = _rotation;
    return *this;
}

AbstractInstance& AbstractInstance::setScale(const glm::vec3& _scale) noexcept {
    scale = _scale;
    return *this;
}

void AbstractInstance::draw(MaterialShader material_shader_type, Blending blending) {
    draw(material_shader_type, blending, UniformSetter{});
}

void AbstractInstance::update() {
    EffectAttachable::update();
}

AbstractInstance::AbstractInstance(Lighting* _lighting, ModelShader _shader_type, const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale) noexcept
    : EffectAttachable{}, LightAttachable{_lighting}, id{next_id++}, shader_type{_shader_type}, position{_position}, rotation{_rotation}, scale{_scale} {

}

AbstractInstance::AbstractInstance(ModelShader _shader_type, const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale) noexcept
    : EffectAttachable{}, id{next_id++}, shader_type{_shader_type}, position{_position}, rotation{_rotation}, scale{_scale} {

}
