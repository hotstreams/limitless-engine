#include <effect.hpp>

using namespace GraphicsEngine;

void Effect::setPosition(const glm::vec3& position) noexcept {
    for (auto& [name, emitter] : emitters) {
        emitter->setPosition(position);
    }
}

void Effect::setRotation(const glm::vec3& rotation) noexcept {
    for (auto& [name, emitter] : emitters) {
        emitter->setRotation(rotation);
    }
}

SpriteEmitter& Effect::getSpriteEmitter(const std::string& emitter) {
    try {
        auto& emi =  emitters.at(emitter);
        if (emi->getType() != EmitterType::Sprite) {
            throw;
        }
        return static_cast<SpriteEmitter&>(*emi);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to get emitter " + emitter);
    }
}

MeshEmitter& Effect::getMeshEmitter(const std::string& emitter) {
    try {
        auto& emi =  emitters.at(emitter);
        if (emi->getType() != EmitterType::Mesh) {
            throw;
        }
        return static_cast<MeshEmitter&>(*emi);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to get emitter " + emitter);
    }
}

bool Effect::isDone() const noexcept {
    bool done = true;
    for (const auto& [name, emitter] : emitters) {
        done &= emitter->isDone();
    }
    return done;
}

Effect::Effect(const Effect& e) noexcept {
    emitters.reserve(e.emitters.size());
    for (const auto& [name, emitter] : e.emitters) {
        emitters.emplace(name, emitter->clone());
    }
}

Effect& Effect::operator=(const Effect &e) noexcept {
    auto effect = e;
    *this = std::move(effect);
    return *this;
}

void Effect::update() {
    for (auto& [name, emitter] : emitters) {
        emitter->update();
    }
}
