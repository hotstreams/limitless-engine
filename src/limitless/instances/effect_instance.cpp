#include <limitless/instances/effect_instance.hpp>
#include <limitless/fx/emitters/mesh_emitter.hpp>
#include <limitless/pipeline/postprocessing.hpp>
#include <glm/gtx/matrix_decompose.hpp>

using namespace Limitless;

void Limitless::swap(EffectInstance& lhs, EffectInstance& rhs) noexcept {
    std::swap(lhs.emitters, rhs.emitters);
}

bool EffectInstance::isDone() const noexcept {
    bool done = true;
    for (const auto& [_, emitter] : emitters) {
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

void EffectInstance::updateEmitters(Context& context, const Camera& camera) const noexcept {
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
		emitter->update(context, camera);
	}
}

void EffectInstance::update(Context& context, const Camera& camera) {
    AbstractInstance::update(context, camera);
	updateEmitters(context, camera);
	done = isDone();
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

EffectInstance::EffectInstance(const std::shared_ptr<EffectInstance>& effect, const glm::vec3& position) noexcept
	: AbstractInstance(ModelShader::Effect, position) {
	emitters.reserve(effect->emitters.size());
	for (const auto& [name, emitter] : effect->emitters) {
		emitters.emplace(name, emitter->clone());
	}

	EffectInstance::setPosition(position);
	EffectInstance::setRotation(rotation);
}

EffectInstance::EffectInstance() noexcept
    : AbstractInstance(ModelShader::Effect, glm::vec3{0.f}) {
}

void EffectInstance::updateBoundingBox() noexcept {
    //TODO:
}
