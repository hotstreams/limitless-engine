#include <limitless/instances/light_attachable.hpp>

using namespace LimitlessEngine;

LightAttachable::LightAttachable(Lighting* _lighting) noexcept
    : lighting{_lighting} {
}

LightAttachable::~LightAttachable() {
    for (const auto& [id, offset] : point_lights) {
        lighting->point_lights.erase(id);
    }

//    for (const auto& [id, offset] : spot_lights)
//        lighting->spot_lights.erase(id);
}

void LightAttachable::setPosition(const glm::vec3& position) noexcept {
    for (const auto& [id, offset] : point_lights)
        lighting->point_lights[id].position = { position + offset, 1.0f };
}

void LightAttachable::setRotation([[maybe_unused]] const glm::quat& rotation) noexcept {
}

void LightAttachable::rotateBy([[maybe_unused]] const glm::quat& rotation) noexcept {
}
