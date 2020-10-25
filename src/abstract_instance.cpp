#include <abstract_instance.hpp>

using namespace GraphicsEngine;

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
