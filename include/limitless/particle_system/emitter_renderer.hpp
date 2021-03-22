#pragma once

#include <limitless/particle_system/emitter_visitor.hpp>
#include <limitless/core/vertex_array.hpp>
#include <memory>

namespace LimitlessEngine {
    class Emitter;
    class Assets;

    static constexpr auto EMITTER_STORAGE_INSTANCE_COUNT = 5;

    class SpriteEmitterRenderer {
    private:
        VertexArray vertex_array;
        std::unique_ptr<Buffer> buffer;
        uint64_t max_particle_count {};

        uint64_t current_particle_count {};
        void checkStorageSize(uint64_t count);
    public:
        explicit SpriteEmitterRenderer(const Emitter& emitter, uint64_t emitter_instance_count = EMITTER_STORAGE_INSTANCE_COUNT);

        void update(SpriteParticleCollector& collector);
        void draw(const Assets& assets, const UniqueSpriteEmitter& emitter);
    };

    class MeshEmitterRenderer {
    private:
        std::shared_ptr<Buffer> buffer;
        uint64_t max_particle_count {};

        uint64_t current_particle_count {};
        void checkStorageSize(uint64_t count);
    public:
        explicit MeshEmitterRenderer(const Emitter& emitter, uint64_t emitter_instance_count = EMITTER_STORAGE_INSTANCE_COUNT);

        void update(MeshParticleCollector& collector);
        void draw(const Assets& assets, const UniqueMeshEmitter& emitter) const;
    };

    VertexArray& operator<<(VertexArray& vertex_array, const std::pair<Particle, Buffer&>& attribute) noexcept;
}
