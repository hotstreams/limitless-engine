#include <limitless/lighting/light.hpp>

using namespace Limitless;

Light::Light(const glm::vec4& color, const glm::vec3& position, float radius) noexcept
    : color {color}
    , position {position}
    , direction {}
    , inner_outer_angles {}
    , id {next_id++}
    , radius {radius}
    , type {Type::Point}
    , changed {true}
    , removed {}
    , hidden {} {
}

Light::Light(const glm::vec4& color, const glm::vec3& position, const glm::vec3& direction, const glm::vec2& inner_outer_angles, float radius) noexcept
    : color {color}
    , position {position}
    , direction {direction}
    , inner_outer_angles {inner_outer_angles}
    , id {next_id++}
    , radius {radius}
    , type {Type::Spot}
    , changed {true}
    , removed {}
    , hidden {} {
}

Light::Light(const glm::vec4& color, const glm::vec3& direction) noexcept
    : color {color}
    , position {}
    , direction {direction}
    , inner_outer_angles {}
    , id {next_id++}
    , radius {}
    , type {Type::DirectionalLight}
    , changed {true}
    , removed {}
    , hidden {} {
}

Light::Light(const Light& light) noexcept
    : color {light.color}
    , position {light.position}
    , direction {light.direction}
    , inner_outer_angles {light.inner_outer_angles}
    , id {next_id++}
    , radius {light.radius}
    , type {light.type}
    , changed {true}
    , removed {light.removed}
    , hidden {light.hidden} {
}

Light::Light(Light&& light) noexcept
    : color {light.color}
    , position {light.position}
    , direction {light.direction}
    , inner_outer_angles {light.inner_outer_angles}
    , id {light.id}
    , radius {light.radius}
    , type {light.type}
    , changed {true}
    , removed {light.removed}
    , hidden {light.hidden} {
}

Light& Light::operator=(const Light& light) noexcept {
    color = light.color;
    position = light.position;
    direction = light.direction;
    inner_outer_angles = light.inner_outer_angles;
    id = next_id++;
    radius = light.radius;
    type = light.type;
    changed = true;
    removed = light.removed;
    hidden = light.hidden;
    return *this;
}

Light& Light::operator=(Light&& light) noexcept {
    color = light.color;
    position = light.position;
    direction = light.direction;
    inner_outer_angles = light.inner_outer_angles;
    id = light.id;
    radius = light.radius;
    type = light.type;
    changed = true;
    removed = light.removed;
    hidden = light.hidden;
    return *this;
}

const glm::vec4& Light::getColor() const noexcept {
    return color;
}

void Light::setColor(const glm::vec4& _color) noexcept {
    color = _color;
    change();
}

glm::vec3 Light::getPosition() const noexcept {
    return position;
}

glm::vec3 &Light::getPosition() noexcept {
    change();
    return position;
}

void Light::setPosition(const glm::vec3& _position) noexcept {
    position = _position;
    change();
}

glm::vec3 Light::getDirection() const noexcept {
    return direction;
}

void Light::setDirection(const glm::vec3& _direction) noexcept {
    direction = _direction;
    change();
}

glm::vec2 Light::getCone() const noexcept {
    return inner_outer_angles;
}

void Light::setCone(const glm::vec2& _inner_outer_angles) noexcept {
    inner_outer_angles = _inner_outer_angles; change();
}

float Light::getRadius() const noexcept {
    return radius;
}

void Light::setRadius(float _radius) noexcept {
    radius = _radius;
    change();
}

Light::Type Light::getType() const noexcept {
    return type;
}

bool Light::isDirectional() const noexcept {
    return type == Type::DirectionalLight;
}

bool Light::isPoint() const noexcept {
    return type == Type::Point;
}

bool Light::isSpot() const noexcept {
    return type == Type::Spot;
}

uint64_t Light::getId() const noexcept {
    return id;
}

bool Light::isChanged() const noexcept {
    return changed;
}

void Light::resetChanged() noexcept {
    changed = false;
}

void Light::change() noexcept {
    changed = true;
}

bool Light::isRemoved() const noexcept {
    return removed;
}

void Light::remove() noexcept {
    removed = true;
    change();
}

bool Light::isHidden() const noexcept {
    return hidden;
}

void Light::hide() noexcept {
    hidden = true;
    change();
}

void Light::reveal() noexcept {
    hidden = false;
    change();
}

bool Light::Builder::isSpot() {
    return color_ != glm::vec4(0.0f) && position_ != glm::vec3(0.0) && inner_angle_ != 0.0f && outer_angle_ != 0.0f && direction_ != glm::vec3(0.0f) && radius_ != 0.0f;
}

bool Light::Builder::isPoint() {
    return color_ != glm::vec4(0.0f) && position_ != glm::vec3(0.0) && radius_ != 0.0f;
}

bool Light::Builder::isDirectional() {
    return color_ != glm::vec4(0.0f) && direction_ != glm::vec3(0.0f);
}

Light::Builder& Light::Builder::color(const glm::vec4& _color) noexcept {
    color_ = _color;
    return *this;
}

Light::Builder& Light::Builder::position(const glm::vec3& _position) noexcept {
    position_ = _position;
    return *this;
}

Light::Builder& Light::Builder::direction(const glm::vec3& _direction) noexcept {
    direction_ = _direction;
    return *this;
}

Light::Builder& Light::Builder::cone(float outer_angle, float inner_angle) noexcept {
    outer_angle_ = outer_angle;
    inner_angle_ = inner_angle;
    return *this;
}

Light::Builder& Light::Builder::radius(float _radius) noexcept {
    radius_ = _radius;
    return *this;
}

Light Light::Builder::build() {
    if (isSpot()) {
        if (inner_angle_ < 0.00872665f || inner_angle_ > 90.0f || outer_angle_ < inner_angle_ || outer_angle_ > 90.0f) {
            throw light_builder_exception {"Inner/Outer angles are out of range!"};
        }
        return {color_, position_, direction_, {inner_angle_, outer_angle_}, radius_};
    }

    if (isPoint()) {
        return {color_, position_, radius_};
    }

    if (isDirectional()) {
        return {color_, direction_};
    }

    throw light_builder_exception {"Required parameters are not set!"};
}

Light Light::Builder::buildDefaultDirectional() {
    direction({-1.0f, -1.0f, -1.0f});
    color({1.0f, 1.0f, 1.0f, 1.0f});
    return build();
}

Light::Builder Light::builder() {
    return {};
}
