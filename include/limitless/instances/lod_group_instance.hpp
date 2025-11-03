#pragma once

#include <limitless/instances/mesh_instance.hpp>
#include <limitless/camera.hpp>
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>

namespace Limitless {
    class LodGroupInstance final {
    private:
        using Lod = std::unordered_map<std::string, MeshInstance>;

        std::vector<Lod> lods;
        uint32_t current_lod;

        LodSelection lod_selection;
        LodTransition lod_transition;
        std::vector<float> distances;

        void selectLod(const Camera& camera, const glm::vec3& position);

    public:
        LodGroupInstance(const std::shared_ptr<Model>& model);

        void update(const Camera& camera, const glm::vec3& position);

        [[nodiscard]] const auto& getLods() const noexcept { return lods; }
        [[nodiscard]] uint32_t getCurrentLod() const noexcept { return current_lod; }
        [[nodiscard]] const auto& getCurrentMeshes() const noexcept { return lods[current_lod]; }    
        [[nodiscard]] auto& getCurrentMeshes() noexcept { return lods[current_lod]; }
    };
}