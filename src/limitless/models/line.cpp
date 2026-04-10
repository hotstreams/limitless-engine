#include <limitless/models/line.hpp>

#include <limitless/models/mesh_builder.hpp>
#include <limitless/core/vertex.hpp>
#include <limitless/core/vertex_stream/vertex_stream_builder.hpp>

using namespace Limitless;

Line::Line(const glm::vec3 &a, const glm::vec3 &b)
    : Model(
        "line",
        {[&a, &b]() {
            std::vector<Vertex> vertices = {
            { a,  glm::vec2(0.0f) },
            { b,  glm::vec2(0.0f) }
            };

            return Mesh::builder()
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
            .build();
        }()},
        {nullptr},
        LodTransition::None,
        LodSelection::CameraDistance,
        {0.0f},
        0.25f
    ) {
}