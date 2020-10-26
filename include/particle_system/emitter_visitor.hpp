#pragma once

#include <particle_system/unique_emitter.hpp>
#include <particle_system/particle.hpp>
#include <map>
#include <vector>

namespace GraphicsEngine {
    class SpriteEmitter;
    class MeshEmitter;

    class SpriteEmitterRenderer;
    class MeshEmitterRenderer;

    class EmitterVisitor {
    public:
        virtual void visit(const SpriteEmitter& emitter) noexcept = 0;
        virtual void visit(const MeshEmitter& emitter) noexcept = 0;

        virtual ~EmitterVisitor() = default;
    };

    class SpriteParticleCollector : public EmitterVisitor {
    private:
        std::vector<Particle> particles;
        const UniqueSpriteEmitter& unique_emitter;
    public:
        explicit SpriteParticleCollector(const UniqueSpriteEmitter& emitter) noexcept;
        ~SpriteParticleCollector() override = default;

        void visit(const SpriteEmitter& emitter) noexcept override;
        void visit(const MeshEmitter& emitter) noexcept override;

        auto&& yield() noexcept { return std::move(particles); }
    };

    class MeshParticleCollector : public EmitterVisitor {
    private:
        std::vector<MeshParticle> particles;
        const UniqueMeshEmitter& unique_emitter;
    public:
        explicit MeshParticleCollector(const UniqueMeshEmitter& emitter) noexcept;
        ~MeshParticleCollector() override = default;

        void visit(const SpriteEmitter& emitter) noexcept override;
        void visit(const MeshEmitter& emitter) noexcept override;

        auto&& yield() noexcept { return std::move(particles); }
    };

    class EmitterStorageFiller : public EmitterVisitor {
    private:
        std::map<UniqueSpriteEmitter, SpriteEmitterRenderer>& sprite_storage;
        std::map<UniqueMeshEmitter, MeshEmitterRenderer>& mesh_storage;
    public:
        EmitterStorageFiller(decltype(sprite_storage) sprite, decltype(mesh_storage) mesh) noexcept;
        ~EmitterStorageFiller() override = default;

        void visit(const SpriteEmitter& emitter) noexcept override;
        void visit(const MeshEmitter& emitter) noexcept override;
    };
}