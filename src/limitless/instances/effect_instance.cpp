#include <limitless/instances/effect_instance.hpp>
#include <limitless/fx/emitters/mesh_emitter.hpp>
#include <glm/gtx/matrix_decompose.hpp>

using namespace Limitless;

bool EffectInstance::isDone() const noexcept {
    bool done = true;
    for (const auto& [_, emitter] : emitters) {
        done &= emitter->isDone();
    }
    return done;
}

void EffectInstance::updateEmitters(const Camera& camera) const noexcept {
	// because we do not use final_matrix in emitter shaders explicitly
	// we should decompose it to parameters
	// and set it to emitters
	glm::vec3 translation {0.0f};
	glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};
	glm::vec3 scale {1.0f};
	glm::vec3 skew {0.0f};
	glm::vec4 perspective {1.0f};

	glm::decompose(final_matrix, scale, rotation, translation, skew, perspective);
	// gets inverted rotation, so we fix it
	rotation = glm::conjugate(rotation);

	for (auto& [_, emitter] : emitters) {
		emitter->setPosition(translation);
		emitter->setRotation(rotation);
		//TODO
		//emitter->setScale(scale);
        emitter->update(camera);
	}
}

EffectInstance::EffectInstance() noexcept
    : Instance(InstanceType::Effect, glm::vec3{0.0f}) {
}

EffectInstance::EffectInstance(const std::shared_ptr<EffectInstance>& effect, const glm::vec3& position) noexcept
    : Instance(InstanceType::Effect, position) {
    emitters.reserve(effect->emitters.size());
    for (const auto& [emitter_name, emitter] : effect->emitters) {
        emitters.emplace(emitter_name, emitter->clone());
    }
}

EffectInstance::EffectInstance(const EffectInstance& effect) noexcept
    : Instance {effect} {
    emitters.reserve(effect.emitters.size());
    for (const auto& [emitter_name, emitter] : effect.emitters) {
        emitters.emplace(emitter_name, emitter->clone());
    }
}

std::unique_ptr<Instance> EffectInstance::clone() noexcept {
    return std::make_unique<EffectInstance>(*this);
}

void EffectInstance::update(const Camera &camera) {
    Instance::update(camera);
	updateEmitters(camera);
	done = isDone();
}
