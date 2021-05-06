#include <limitless/instances/effect_instance.hpp>
#include <limitless/fx/emitters/mesh_emitter.hpp>
#include <limitless/pipeline/shader_pass_types.hpp>
#include <limitless/pipeline/postprocessing.hpp>

using namespace Limitless;

void Limitless::swap(EffectInstance& lhs, EffectInstance& rhs) noexcept {
    std::swap(lhs.emitters, rhs.emitters);
}

EffectInstance& EffectInstance::setPosition(const glm::vec3& _position) noexcept {
    AbstractInstance::setPosition(_position);

    for (auto& [name, emitter] : emitters) {
        emitter->setPosition(position);
    }

    return *this;
}

EffectInstance& EffectInstance::setRotation(const glm::quat& _rotation) noexcept {
    AbstractInstance::setRotation(_rotation);

    for (auto& [name, emitter] : emitters) {
        emitter->setRotation(rotation);
    }

    return *this;
}

EffectInstance& EffectInstance::rotateBy(const glm::quat& _rotation) noexcept {
    AbstractInstance::rotateBy(_rotation);

    for (auto& [name, emitter] : emitters) {
        emitter->setRotation(rotation);
    }

    return *this;
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

void EffectInstance::update(Context& context, Camera& camera) {
    AbstractInstance::update(context, camera);
    done = isDone();

    for (auto& [name, emitter] : emitters) {
        emitter->update(context, camera);
    }
}

void EffectInstance::draw([[maybe_unused]] Limitless::Context& ctx,
                          [[maybe_unused]] const Assets& assets,
                          [[maybe_unused]] ShaderPass shader_type,
                          [[maybe_unused]] ms::Blending blending,
                          [[maybe_unused]] const UniformSetter& uniform_set) {
    // One does not simply render effect instance!
    // use EffectRenderer
}

EffectInstance* EffectInstance::clone() noexcept {
    return new EffectInstance(*this);
}

EffectInstance::EffectInstance(Lighting *lighting, const std::shared_ptr<EffectInstance>& effect, const glm::vec3& position, const glm::vec3& rotation) noexcept
    : AbstractInstance{lighting, ModelShader::Effect, position, rotation, glm::vec3{0.0f}} {
    emitters.reserve(effect->emitters.size());
    for (const auto& [name, emitter] : effect->emitters) {
        emitters.emplace(name, emitter->clone());
    }

	EffectInstance::setPosition(position);
	EffectInstance::setRotation(rotation);
}

EffectInstance::EffectInstance(const std::shared_ptr<EffectInstance>& effect, const glm::vec3& position, const glm::vec3& rotation) noexcept
    : EffectInstance{nullptr, effect, position, rotation}  {
	EffectInstance::setPosition(position);
	EffectInstance::setRotation(rotation);
}

EffectInstance::EffectInstance() noexcept
    : AbstractInstance{ModelShader::Effect, glm::vec3{0.f}, glm::vec3{0.f}, glm::vec3{1.f}} {
}

void EffectInstance::calculateBoundingBox() noexcept {
    //TODO:
}
