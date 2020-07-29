#pragma once

#include <sprite_emitter.hpp>
#include <mesh.hpp>

namespace GraphicsEngine {
    struct UniqueMeshEmitter {
        UniqueSpriteEmitter unique_sprite_emitter;
        std::shared_ptr<AbstractMesh> mesh;
    };
    bool operator<(const UniqueMeshEmitter& lhs, const UniqueMeshEmitter& rhs) noexcept;
    bool operator==(const UniqueMeshEmitter& lhs, const UniqueMeshEmitter& rhs) noexcept;

    struct MeshParticle {
        glm::mat4 model;
        glm::vec4 color;
        glm::vec4 subUV;
        glm::vec4 properties;
    };

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
    };
}