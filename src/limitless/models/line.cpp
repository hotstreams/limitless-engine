#include <limitless/models/line.hpp>

using namespace Limitless;

#include <limitless/models/mesh.hpp>

Line::Line(const glm::vec3 &a, const glm::vec3 &b) : ElementaryModel("line") {
    std::vector<Vertex> vertices = {
            { a,  glm::vec2(0.0f) },
            { b,  glm::vec2(0.0f) }
    };

    meshes.emplace_back(new Mesh(std::move(vertices), "line", MeshDataType::Static, DrawMode::Lines));
    calculateBoundingBox();
}