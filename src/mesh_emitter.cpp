#include <mesh_emitter.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace GraphicsEngine;

bool GraphicsEngine::operator<(const UniqueMeshEmitter& lhs, const UniqueMeshEmitter& rhs) noexcept {
    return std::tie(lhs.unique_sprite_emitter, lhs.mesh) < std::tie(rhs.unique_sprite_emitter, rhs.mesh);
}

bool GraphicsEngine::operator==(const UniqueMeshEmitter& lhs, const UniqueMeshEmitter& rhs) noexcept {
    return lhs.unique_sprite_emitter == rhs.unique_sprite_emitter && lhs.mesh == rhs.mesh;
}

MeshEmitter::MeshEmitter() noexcept
    : SpriteEmitter() {
    type = EmitterType::Mesh;
}

MeshEmitter* MeshEmitter::clone() const noexcept {
    return new MeshEmitter(*this);
}

void MeshEmitter::update() {
    if (done) {
        return;
    }

    Emitter::update();

    mesh_particles.clear();
    mesh_particles.reserve(particles.size());
    for (const auto& particle : particles) {
        auto model = glm::translate(glm::mat4(1.0f), particle.position);

        model = glm::rotate(model, particle.angle.x, glm::vec3(1.0f, 0.f, 0.f));
        model = glm::rotate(model, particle.angle.y, glm::vec3(0.0f, 1.f, 0.f));
        model = glm::rotate(model, particle.angle.z, glm::vec3(0.0f, 0.f, 1.f));

        model = glm::scale(model, glm::vec3(particle.size));

        mesh_particles.emplace_back(MeshParticle{model, particle.color, particle.subUV, particle.properties});
    }
}

UniqueMeshEmitter MeshEmitter::getEmitterType() const noexcept {
    return { SpriteEmitter::getEmitterType(), mesh };
}

