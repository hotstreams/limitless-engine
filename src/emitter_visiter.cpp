#include <emitter_visiter.hpp>
#include <emitter_renderer.hpp>
#include <unique_emitter.hpp>
#include <mesh_emitter.hpp>

using namespace GraphicsEngine;

SpriteParticleCollector::SpriteParticleCollector(const UniqueSpriteEmitter& emitter) noexcept : unique_emitter{emitter} {

}

void SpriteParticleCollector::visit(const SpriteEmitter& emitter) noexcept {
    if (unique_emitter == emitter.getEmitterType()) {
        particles.insert(particles.end(), emitter.getParticles().begin(), emitter.getParticles().end());
    }
}

void SpriteParticleCollector::visit([[maybe_unused]] const MeshEmitter& emitter) noexcept {

}

MeshParticleCollector::MeshParticleCollector(const UniqueMeshEmitter& emitter) noexcept : unique_emitter{emitter} {

}

void MeshParticleCollector::visit([[maybe_unused]] const SpriteEmitter& emitter) noexcept {

}

void MeshParticleCollector::visit(const MeshEmitter& emitter) noexcept {
    if (unique_emitter == emitter.getEmitterType()) {
        particles.insert(particles.end(), emitter.getMeshParticles().begin(), emitter.getMeshParticles().end());
    }
}

EmitterStorageFiller::EmitterStorageFiller(decltype(sprite_storage) sprite, decltype(mesh_storage) mesh) noexcept
    : sprite_storage{sprite}, mesh_storage{mesh} {

}

void EmitterStorageFiller::visit(const SpriteEmitter& emitter) noexcept {
    const auto unique_emitter = emitter.getEmitterType();
    if (sprite_storage.count(unique_emitter) == 0) {
        sprite_storage.emplace(emitter.getEmitterType(), SpriteEmitterRenderer{emitter});
    }
}

void EmitterStorageFiller::visit(const MeshEmitter& emitter) noexcept {
    const auto unique_emitter = emitter.getEmitterType();
    if (mesh_storage.count(unique_emitter) == 0) {
        mesh_storage.emplace(emitter.getEmitterType(), MeshEmitterRenderer{emitter});
    }
}
