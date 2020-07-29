#pragma once

#include <effect.hpp>
#include <emitter_buffer.hpp>

namespace GraphicsEngine {
    class ParticleSystem {
    private:
        std::unordered_map<uint64_t, std::unique_ptr<Effect>> effects;

        std::map<UniqueSpriteEmitter, SpriteEmitterBuffer> sprite_buffers;
        std::map<UniqueMeshEmitter, MeshEmitterBuffer> mesh_buffers;

        std::vector<Particle> collectSpriteParticles(const UniqueSpriteEmitter& type) const;
        std::vector<MeshParticle> collectMeshParticles(const UniqueMeshEmitter& type) const;
    public:
        void update();

        void draw(Blending blending);

        void addInstance(size_t id, Effect* effect);
        void removeInstance(size_t id);
    };
}