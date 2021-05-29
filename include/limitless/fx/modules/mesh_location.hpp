#pragma once

#include <limitless/fx/modules/module.hpp>
#include <limitless/models/mesh.hpp>

namespace Limitless::fx {
    template<typename Particle>
    class MeshLocation : public Module<Particle> {
    private:
        std::shared_ptr<AbstractMesh> mesh;

        std::default_random_engine generator;
        std::uniform_real_distribution<float> distribution;

        glm::vec3 getPositionOnTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) noexcept {
            const auto r1 = std::sqrt(distribution(generator));
            const auto r2 = distribution(generator);
            const auto m1 = 1.0f - r1;
            const auto m2 = r1 * (1.0f - r2);
            const auto m3 = r2 * r1;

            return (m1 * a) + (m2 * b) + (m3 * c);
        }

        glm::vec3 getPositionOnMesh() noexcept {
            const auto& vertices = static_cast<Mesh<VertexNormalTangent>&>(*mesh).getVertices();

            using vector_size_type = typename std::remove_reference_t<decltype(vertices)>::size_type;
            auto distr = std::uniform_int_distribution(static_cast<vector_size_type>(0), vertices.size() - 3);

            const auto index = distr(generator);
            return getPositionOnTriangle(vertices[index].position, vertices[index + 1].position, vertices[index + 2].position);
        }
    public:
        explicit MeshLocation(std::shared_ptr<AbstractMesh> _mesh) noexcept
            : Module<Particle>(ModuleType::MeshLocation)
            , mesh{std::move(_mesh)}
            , distribution(0.0f, 1.0f) {
        }

        ~MeshLocation() override = default;

        MeshLocation(const MeshLocation&) = default;
        MeshLocation& operator=(const MeshLocation&) noexcept = default;

        auto& getMesh() noexcept { return mesh; }
        const auto& getMesh() const noexcept { return mesh; }

        void initialize([[maybe_unused]] AbstractEmitter& emitter, Particle& particle) noexcept override {
            particle.getPosition() += getPositionOnMesh();
        }

        void update([[maybe_unused]] AbstractEmitter& emitter, [[maybe_unused]] std::vector<Particle>& particles, [[maybe_unused]] float dt, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Camera& camera) noexcept override {
            //TODO: skeletal mesh update
        }

        [[nodiscard]] MeshLocation* clone() const noexcept override {
            return new MeshLocation(*this);
        }
    };
}