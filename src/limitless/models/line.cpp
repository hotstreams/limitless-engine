#include <limitless/models/line.hpp>

using namespace LimitlessEngine;

#include <limitless/models/mesh.hpp>

Line::Line(const glm::vec3 &a, const glm::vec3 &b) {
    std::vector<Vertex> vertices = {
            { a,  glm::vec2(0.0f) },
            { b,  glm::vec2(0.0f) }
    };

    meshes.emplace_back(new Mesh(std::move(vertices), "line_mesh", MeshDataType::Static, DrawMode::Lines));
    calculateBoundingBox();
}