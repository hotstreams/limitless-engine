#pragma once

#include <sprite_emitter.hpp>
#include <mesh.hpp>

namespace GraphicsEngine {
    struct UniqueMeshEmitter;

    class MeshEmitter : public SpriteEmitter {
    private:
        std::shared_ptr<AbstractMesh> mesh;

        std::vector<MeshParticle> mesh_particles;
        MeshEmitter() noexcept;
    public:
        ~MeshEmitter() override = default;

        MeshEmitter(const MeshEmitter&) = default;
        MeshEmitter& operator=(const MeshEmitter&) noexcept = default;

        MeshEmitter(MeshEmitter&&) noexcept = default;
        MeshEmitter& operator=(MeshEmitter&&) noexcept = default;

        [[nodiscard]] MeshEmitter* clone() const noexcept override;

        [[nodiscard]] const auto& getMesh() const noexcept { return mesh; }
        [[nodiscard]] const auto& getMeshParticles() const noexcept { return mesh_particles; }
        [[nodiscard]] UniqueMeshEmitter getEmitterType() const noexcept;

        void update() override;

        void accept(EmitterVisiter& visiter) noexcept override;
    };
}