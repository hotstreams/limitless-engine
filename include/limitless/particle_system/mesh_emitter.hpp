#pragma once

#include <limitless/particle_system/sprite_emitter.hpp>
#include <limitless/mesh.hpp>

namespace LimitlessEngine {
    struct UniqueMeshEmitter;

    class MeshEmitter : public SpriteEmitter {
    private:
        std::vector<MeshParticle> mesh_particles;
        std::shared_ptr<AbstractMesh> mesh;

        friend class EffectBuilder;
        MeshEmitter() noexcept;
    public:
        ~MeshEmitter() override = default;

        MeshEmitter(const MeshEmitter&) = default;
        MeshEmitter& operator=(const MeshEmitter&) = default;

        MeshEmitter(MeshEmitter&&) noexcept = default;
        MeshEmitter& operator=(MeshEmitter&&) noexcept = default;

        [[nodiscard]] MeshEmitter* clone() const noexcept override;

        [[nodiscard]] const auto& getMesh() const noexcept { return mesh; }
        [[nodiscard]] const auto& getMeshParticles() const noexcept { return mesh_particles; }
        [[nodiscard]] UniqueMeshEmitter getEmitterType() const noexcept;

        void update() override;

        void accept(EmitterVisitor& visitor) noexcept override;
    };
}