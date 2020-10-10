#include <effect_instance.hpp>
#include <mesh_emitter.hpp>
#include <shader_types.hpp>

using namespace GraphicsEngine;

void GraphicsEngine::swap(EffectInstance& lhs, EffectInstance& rhs) noexcept {
    std::swap(lhs.emitters, rhs.emitters);
}

EffectInstance& EffectInstance::setPosition(const glm::vec3& _position) noexcept {
    AbstractInstance::setPosition(_position);

    for (auto& [name, emitter] : emitters) {
        emitter->setPosition(position);
    }

    return *this;
}

EffectInstance& EffectInstance::setRotation(const glm::vec3& _rotation) noexcept {
    AbstractInstance::setRotation(_rotation);

    for (auto& [name, emitter] : emitters) {
        emitter->setRotation(rotation);
    }

    return *this;
}

SpriteEmitter& EffectInstance::getSpriteEmitter(const std::string& emitter) {
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

MeshEmitter& EffectInstance::getMeshEmitter(const std::string& emitter) {
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

Emitter& EffectInstance::operator[](const std::string& emitter) {
    return *emitters.at(emitter);
}

bool EffectInstance::isDone() const noexcept {
    bool done = true;
    for (const auto& [name, emitter] : emitters) {
        done &= emitter->isDone();
    }
    return done;
}

EffectInstance::EffectInstance(const EffectInstance& effect) noexcept
    : AbstractInstance{effect} {
    emitters.reserve(effect.emitters.size());
    for (const auto& [name, emitter] : effect.emitters) {
        emitters.emplace(name, emitter->clone());
    }
}

EffectInstance& EffectInstance::operator=(const EffectInstance& effect) noexcept {
    auto copied = EffectInstance{effect};
    *this = std::move(copied);
    return *this;
}

void EffectInstance::update() {
    AbstractInstance::update();
    // updates end of lifetime
    done = isDone();

    for (auto& [name, emitter] : emitters) {
        emitter->update();
    }
}

void EffectInstance::draw([[maybe_unused]] MaterialShader shader_type, [[maybe_unused]] Blending blending, [[maybe_unused]] const UniformSetter& uniform_set) {
    //throw std::runtime_error("One does not simply render effect instance!");
}

EffectInstance::~EffectInstance() {

}

EffectInstance* EffectInstance::clone() noexcept {
    return new EffectInstance(*this);
}