#include <limitless/models/line.hpp>

using namespace Limitless;

#include <limitless/models/mesh.hpp>

Line::Line(const glm::vec3 &a, const glm::vec3 &b) : ElementaryModel("line") {
    std::vector<Vertex> vertices = {
            { a,  glm::vec2(0.0f) },
            { b,  glm::vec2(0.0f) }
    };

    meshes.emplace_back(
        std::make_unique<Mesh>(
            std::make_unique<VertexStream<Vertex>>(std::move(vertices), VertexStreamUsage::Static, VertexStreamDraw::Lines),
            "line_mesh")
    );

    calculateBoundingBox();
}