#include <limitless/models/line.hpp>

using namespace Limitless;

#include <limitless/models/mesh_builder.hpp>
#include <limitless/core/vertex.hpp>
#include <limitless/core/vertex_stream/vertex_stream_builder.hpp>

Line::Line(const glm::vec3 &a, const glm::vec3 &b) : ElementaryModel("line") {
    std::vector<Vertex> vertices = {
        { a,  glm::vec2(0.0f) },
        { b,  glm::vec2(0.0f) }
    };

    meshes.emplace_back(
        Mesh::builder()
            .name("line_mesh")
            .vertex_stream(
                VertexStream::builder()
                    .attribute(0, VertexStream::Attribute::Position, sizeof(Vertex), offsetof(Vertex, position))
                    .attribute(1, VertexStream::Attribute::Uv, sizeof(Vertex), offsetof(Vertex, uv))
                    .vertices(vertices)
                    .usage(VertexStream::Usage::Static)
                    .draw(VertexStream::Draw::Lines)
                    .build()
            )
            .build()
    );

    calculateBoundingBox();
}