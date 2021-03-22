#pragma once

#include <limitless/particle_system/emitter_visitor.hpp>
#include <limitless/core/vertex_array.hpp>
#include <memory>

namespace LimitlessEngine {
    class Emitter;

    static constexpr auto emitter_storage_instance_count = 5;

    class SpriteEmitterRenderer {
    private:
        VertexArray vertex_array;
        std::unique_ptr<Buffer> buffer;
        uint64_t max_particle_count {0};

        uint64_t current_particle_count {0};
        void checkStorageSize(uint64_t count);
    public:
        explicit SpriteEmitterRenderer(const Emitter& emitter, uint64_t emitter_instance_count = emitter_storage_instance_count);

        void update(SpriteParticleCollector& collector);
        void draw(const UniqueSpriteEmitter& emitter);
    };

    class MeshEmitterRenderer {
    private:
        std::shared_ptr<Buffer> buffer;
        uint64_t max_particle_count {0};

        uint64_t current_particle_count {0};
        void checkStorageSize(uint64_t count);
    public:
        explicit MeshEmitterRenderer(const Emitter& emitter, uint64_t emitter_instance_count = emitter_storage_instance_count);

        void update(MeshParticleCollector& collector);
        void draw(const UniqueMeshEmitter& emitter);
    };

    VertexArray& operator<<(VertexArray& vertex_array, const std::pair<Particle, Buffer&>& attribute) noexcept;
}
