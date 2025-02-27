#pragma once

#include <limitless/fx/modules/mesh_location.hpp>

namespace Limitless::fx {
    //TODO: restore with new interface
    template<typename Particle>
    class MeshLocationAttachment final : public InitialMeshLocation<Particle> {
    private:
        struct LocationCache {
            std::shared_ptr<Mesh> selected_mesh;
            size_t vertex_index;
            std::pair<float, float> triangle_position;
            glm::vec3 last_position;
        };
        std::map<size_t, LocationCache> cache;
    public:
        explicit MeshLocationAttachment(std::shared_ptr<Mesh> mesh) noexcept
            : InitialMeshLocation<Particle>(ModuleType::MeshLocationAttachment, std::move(mesh)) {
        }

        explicit MeshLocationAttachment(std::shared_ptr<AbstractModel> mesh) noexcept
            : InitialMeshLocation<Particle>(ModuleType::MeshLocationAttachment, std::move(mesh)) {
        }

        ~MeshLocationAttachment() override = default;

        MeshLocationAttachment(const MeshLocationAttachment&) = default;
        MeshLocationAttachment& operator=(const MeshLocationAttachment&) noexcept = default;

        void initialize([[maybe_unused]] AbstractEmitter& emitter, Particle& particle, size_t index) noexcept override {
//            const auto selected_mesh = this->getSelectedMesh();
//            const auto vertex_index = this->getVertexIndex(selected_mesh);
//            const auto triangle_pos = this->getTrianglePosition();
//            const auto mesh_position = this->getPositionOnMesh(selected_mesh, vertex_index, triangle_pos.first, triangle_pos.second);
//            particle.position += mesh_position;
//
//            cache[index] = { selected_mesh, vertex_index, triangle_pos, mesh_position };
        }

        void deinitialize(const std::vector<size_t>& indices) override {
            for (const auto& index : indices) {
                cache.erase(index);
            }

            decltype(cache) new_cache;

            size_t i {};
            for (const auto& [_, c] : cache) {
                new_cache.emplace(i++, c);
            }

            cache = std::move(new_cache);
        }

        MeshLocationAttachment* clone() const noexcept override {
            return new MeshLocationAttachment<Particle>(*this);
        }

        void update([[maybe_unused]] AbstractEmitter &emitter, std::vector<Particle> &particles, [[maybe_unused]] float dt, [[maybe_unused]] const Camera &camera) noexcept override {
            for (size_t i = 0; i < particles.size(); ++i) {
                auto& [selected_mesh, vertex_index, triangle, last_position] = cache[i];
                const auto mesh_position = this->getPositionOnMesh(selected_mesh, vertex_index, triangle.first, triangle.second);
                particles[i].position += mesh_position - last_position;
                last_position = mesh_position;
            }
        }
    };
}
