#include <limitless/fx/emitters/mesh_emitter.hpp>

#include <limitless/fx/emitters/emitter_visitor.hpp>
#include <limitless/fx/modules/module.hpp>

#include <limitless/ms/material.hpp>

using namespace Limitless::fx;

MeshEmitter::MeshEmitter() noexcept
    : Emitter<MeshParticle>(Type::Mesh) {
}

MeshEmitter::MeshEmitter(const MeshEmitter& emitter)
    : Emitter<MeshParticle>(emitter)
    , mesh {emitter.mesh}
    , material {std::make_shared<ms::Material>(*emitter.material)} {
}

MeshEmitter* MeshEmitter::clone() const {
    return new MeshEmitter(*this);
}

void MeshEmitter::update(Context& context, const Camera& camera) {
    if (done) {
        return;
    }

    Emitter::update(context, camera);

    for (auto& particle : particles) {
        auto model = glm::translate(glm::mat4(1.0f), particle.position);

        model = glm::rotate(model, particle.rotation.x, glm::vec3(1.0f, 0.f, 0.f));
        model = glm::rotate(model, particle.rotation.y, glm::vec3(0.0f, 1.f, 0.f));
        model = glm::rotate(model, particle.rotation.z, glm::vec3(0.0f, 0.f, 1.f));

        model = glm::scale(model, glm::vec3(particle.size));

        particle.model = model;
    }
}

void MeshEmitter::accept(EmitterVisitor& visitor) noexcept {
    visitor.visit(*this);
}

UniqueEmitterRenderer MeshEmitter::getUniqueRendererType() const noexcept {
    return { type, mesh, material };
}