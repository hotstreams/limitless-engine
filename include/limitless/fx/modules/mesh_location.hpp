#pragma once

#include <limitless/fx/modules/module.hpp>

namespace Limitless::fx {
//    template<typename Particle>
//    class MeshLocation : public Module<Particle> {
//    private:
//        std::shared_ptr<AbstractMesh> mesh;
//
//        std::default_random_engine generator;
//        std::uniform_real_distribution<float> distribution;
//
//        glm::vec3 getPositionOnTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) noexcept;
//        glm::vec3 getPositionOnMesh() noexcept;
//    public:
//        explicit MeshLocation(std::shared_ptr<AbstractMesh> mesh) noexcept;
//        ~MeshLocation() override = default;
//
//        MeshLocation(const MeshLocation&) noexcept = default;
//        MeshLocation& operator=(const MeshLocation&) noexcept = default;
//
//        auto& getMesh() noexcept { return mesh; }
//
//        void initialize(AbstractEmitter& emitter, Particle& particle) noexcept override;
//        void update(AbstractEmitter& emitter, std::vector<Particle>& particles, float dt) noexcept override;
//
//        [[nodiscard]] MeshLocation* clone() const noexcept override;
//    };
//
//    MeshLocation::MeshLocation(std::shared_ptr<AbstractMesh> _mesh) noexcept
//            : EmitterModule(EmitterModuleType::MeshLocation)
//            , mesh{std::move(_mesh)}
//            , distribution(0.0f, 1.0f) {
//    }
//
//    glm::vec3 MeshLocation::getPositionOnTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) noexcept {
//        const auto r1 = std::sqrt(distribution(generator));
//        const auto r2 = distribution(generator);
//        const auto m1 = 1.0f - r1;
//        const auto m2 = r1 * (1.0f - r2);
//        const auto m3 = r2 * r1;
//
//        return (m1 * a) + (m2 * b) + (m3 * c);
//    }
//
//    glm::vec3 MeshLocation::getPositionOnMesh() noexcept {
//        const auto& vertices = static_cast<Mesh<VertexNormalTangent>&>(*mesh).getVertices();
//
//        using vector_size_type = std::remove_reference_t<decltype(vertices)>::size_type;
//        auto distr = std::uniform_int_distribution(static_cast<vector_size_type>(0), vertices.size() - 3);
//
//        const auto index = distr(generator);
//        return getPositionOnTriangle(vertices[index].position,
//                                     vertices[index + 1].position,
//                                     vertices[index + 2].position);
//    }
//
//    void MeshLocation::initialize([[maybe_unused]] AbstractEmitter& emitter, SpriteParticle& particle) noexcept {
//        particle.position += getPositionOnMesh();
//    }
//
//    void MeshLocation::update([[maybe_unused]] AbstractEmitter& emitter, [[maybe_unused]] std::vector<SpriteParticle>& particles, [[maybe_unused]] float dt) noexcept {
//        //TODO: skeletal mesh update
//    }
//
//    MeshLocation* MeshLocation::clone() const noexcept {
//        return new MeshLocation(*this);
//    }
}