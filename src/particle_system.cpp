#include <particle_system.hpp>

#include <effect.hpp>
#include <sprite_emitter.hpp>
#include <mesh_emitter.hpp>
#include <emitter_buffer.hpp>
#include <material.hpp>

#include <algorithm>

using namespace GraphicsEngine;

std::vector<Particle> ParticleSystem::collectSpriteParticles(const UniqueSpriteEmitter& type) const {
    std::vector<Particle> particles;
    for (const auto& [id, effect] : effects) {
        for (const auto& [name, emitter] : effect->getEmitters()) {
            if (emitter->getType() == EmitterType::Sprite) {
                const auto& sprite_emi = static_cast<SpriteEmitter&>(*emitter);
                if (sprite_emi.getEmitterType() == type && !sprite_emi.isDone()) {
                    const auto& found = sprite_emi.getParticles();
                    particles.insert(particles.end(), found.begin(), found.end());
                }
            }
        }
    }
    return particles;

//    auto collect = [&] (const auto& emitter) {
            // checks and casts sprite emitter
//        if (emitter->getType() == EmitterType::Sprite) {
//            const auto& sprite_emitter = static_cast<const SpriteEmitter&>(*emitter);
                // copies particles for required type
//            if (sprite_emitter.getEmitterType() == type && !sprite_emitter.isDone()) {
//                const auto& found = sprite_emitter.getParticles();
//                std::copy(found.begin(), found.end(), particles.end());
//            }
//        }
//    };
//
//    for (const auto& [id, effect] : effects) {
//        for (const auto& [name, emitter] : effect->getEmitters()) {
//            collect(emitter);
//        }
//    }
}

std::vector<MeshParticle> ParticleSystem::collectMeshParticles(const UniqueMeshEmitter& type) const {
    std::vector<MeshParticle> particles;
    for (const auto& [id, effect] : effects) {
        for (const auto& [name, emitter] : effect->getEmitters()) {
            if (emitter->getType() == EmitterType::Mesh) {
                const auto& mesh_emi = static_cast<MeshEmitter&>(*emitter);
                if (mesh_emi.getEmitterType() == type && !mesh_emi.isDone()) {
                    const auto& found = mesh_emi.getMeshParticles();
                    particles.insert(particles.end(), found.begin(), found.end());
                }
            }
        }
    }
    return particles;
}

void ParticleSystem::update() {
    for (auto& [id, effect] : effects) {
        effect->update();
    }

    for (auto& [unique, buffer] : sprite_buffers) {
        buffer.updateParticles(collectSpriteParticles(unique));
    }

    for (auto& [unique, buffer] : mesh_buffers) {
        buffer.updateParticles(collectMeshParticles(unique));
    }
}

void ParticleSystem::draw(Blending blending) {
    for (auto& [type, buffer] : sprite_buffers) {
        if (type.material->getBlending() != blending)
            continue;

        buffer.draw(type);
    }

    for (auto& [type, buffer] : mesh_buffers) {
        if (type.unique_sprite_emitter.material->getBlending() != blending)
            continue;

        buffer.draw(type, type.mesh);
    }
}

void ParticleSystem::addInstance(size_t id, Effect* effect) {
    effects.emplace(id, effect);

    for (const auto& [name, emitter] : effect->getEmitters()) {
        switch (emitter->getType()) {
            case EmitterType::Sprite: {
                auto& sprite_emitter = static_cast<SpriteEmitter&>(*emitter);
                auto found = sprite_buffers.find(sprite_emitter.getEmitterType());
                if (found != sprite_buffers.end()) {
                    sprite_buffers.emplace(sprite_emitter.getEmitterType(), sprite_emitter);
                } else {
                    ++found->second;
                }
                break;
            }
            case EmitterType::Mesh: {
                auto& mesh_emitter = static_cast<MeshEmitter&>(*emitter);
                auto found = mesh_buffers.find(mesh_emitter.getEmitterType());
                if (found != mesh_buffers.end()) {
                    mesh_buffers.emplace(mesh_emitter.getEmitterType(), mesh_emitter);
                } else {
                    ++found->second;
                }
                break;
            }
        }
    }
}

void ParticleSystem::removeInstance(size_t id) {
    try {
        const auto &effect = effects.at(id);

        for (const auto& [name, emitter] : effect->getEmitters()) {
            switch (emitter->getType()) {
                case EmitterType::Sprite: {
                    auto& sprite_emitter = static_cast<SpriteEmitter&>(*emitter);
                    --sprite_buffers.at(sprite_emitter.getEmitterType());
                    break;
                }
                case EmitterType::Mesh: {
                    auto& mesh_emitter = static_cast<MeshEmitter&>(*emitter);
                    --mesh_buffers.at(mesh_emitter.getEmitterType());
                    break;
                }
            }
        }

        effects.erase(id);
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No such effect instance.");
    }
}
