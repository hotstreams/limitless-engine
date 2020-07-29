#pragma once

#include <emitter.hpp>
#include <mesh_emitter.hpp>

namespace GraphicsEngine {
    VertexArray& operator<<(VertexArray& vertex_array, const std::pair<Particle, Buffer&>& attribute) noexcept;

    class SpriteEmitterBuffer {
    private:
        static constexpr uint32_t block_size = 5;
        uint32_t block_count {1};
        uint32_t count {0};
        uint32_t max_count;
        size_t particles_size {0};

        VertexArray vertex_array;
        std::unique_ptr<Buffer> buffer;
    public:
        explicit SpriteEmitterBuffer(Emitter& emitter);

        SpriteEmitterBuffer& operator--() noexcept;
        SpriteEmitterBuffer& operator++();

        void updateParticles(const std::vector<Particle>& particles);
        void draw(const UniqueSpriteEmitter& unique_emitter) const;
    };

    class MeshEmitterBuffer {
    private:
        static constexpr uint32_t block_size = 5;
        uint32_t block_count {1};
        uint32_t count {0};
        uint32_t max_count;
        size_t particles_size {0};

        std::shared_ptr<Buffer> buffer;
    public:
        explicit MeshEmitterBuffer(Emitter& emitter);

        MeshEmitterBuffer& operator--() noexcept;
        MeshEmitterBuffer& operator++();

        void updateParticles(const std::vector<MeshParticle>& particles);
        void draw(const UniqueMeshEmitter& unique_emitter, const std::shared_ptr<AbstractMesh>& mesh) const;
    };
}
