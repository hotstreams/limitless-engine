#include <limitless/fx/emitters/beam_emitter.hpp>

#include <limitless/fx/emitters/emitter_visitor.hpp>
#include <limitless/fx/modules/beam_builder.hpp>
#include <limitless/ms/material.hpp>

using namespace Limitless::fx;
using namespace Limitless::ms;

BeamEmitter::BeamEmitter() noexcept
    : Emitter<BeamParticle>(AbstractEmitter::Type::Beam) {
}

[[nodiscard]] const std::vector<BeamParticleMapping>& BeamEmitter::getParticles() const noexcept {
    // guaranteed to have beam builder; see EffectBuilder
    const auto& beam_builder = static_cast<BeamBuilder<BeamParticle>&>(**modules.find(std::make_unique<BeamBuilder<BeamParticle>>()));
    return beam_builder.getParticles();
}

BeamEmitter* BeamEmitter::clone() const noexcept {
    return new BeamEmitter(*this);
}

void BeamEmitter::accept(EmitterVisitor& visitor) noexcept {
    visitor.visit(*this);
}

BeamEmitter::BeamEmitter(const BeamEmitter& emitter)
    : Emitter<BeamParticle>(emitter)
    , material {std::make_shared<ms::Material>(*emitter.material)} {
}

UniqueEmitterRenderer BeamEmitter::getUniqueRendererType() const noexcept {
    return { type, std::nullopt, material };
}

