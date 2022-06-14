#pragma once

#include <limitless/models/elementary_model.hpp>
#include <limitless/core/vertex.hpp>

namespace Limitless {
    class Cylinder : public ElementaryModel {
    public:
        [[nodiscard]] std::vector<glm::vec3> generateNormals() const;
        [[nodiscard]] std::vector<glm::vec3> generateUnit() const;
        [[nodiscard]] std::vector<VertexNormalTangent> generate(const std::vector<glm::vec3>& unit, const std::vector<glm::vec3>& normals);
        [[nodiscard]] std::vector<uint32_t> generateIndices();

        float base_radius {1.0f};
        float top_radius {1.0f};
        float height {2.0f};

        uint32_t sector_count {36};
        uint32_t stack_count {1};

        uint32_t base_vertex_index {};
        uint32_t top_vertex_index {};
    public:
        Cylinder();
        Cylinder(float base_radius, float top_radius, float height);
        ~Cylinder() override = default;

        Cylinder(const Cylinder&) = delete;
        Cylinder& operator=(const Cylinder&) = delete;

        Cylinder(Cylinder&&) noexcept = default;
        Cylinder& operator=(Cylinder&&) noexcept = default;
    };
}