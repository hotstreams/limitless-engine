#include <limitless/models/quad.hpp>

using namespace Limitless;

#include <limitless/util/tangent_space.hpp>
#include <limitless/models/mesh_builder.hpp>
#include <limitless/core/vertex_stream/vertex_stream_builder.hpp>


Quad::Quad()
    : Model(
        "quad",
        {[]() {
            std::vector<Vertex> vertices = {
                { {-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f} }, // top-left
                { {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f} }, // bottom-left
                { { 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f} }, // bottom-right
                { { 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f} }  // top-right
            };

            std::vector<uint32_t> indices = {
                0, 1, 2,  // first triangle
                0, 2, 3   // second triangle
            };

            return Mesh::builder()
                .name("quad_mesh")
                .vertex_stream(
                    VertexStream::builder()
                        .attribute(0, VertexStream::Attribute::Position, sizeof(Vertex), offsetof(Vertex, position))
                        .attribute(1, VertexStream::Attribute::Uv, sizeof(Vertex), offsetof(Vertex, uv))
                        .vertices(vertices)
                        .indices(indices)
                        .usage(VertexStream::Usage::Static)
                        .draw(VertexStream::Draw::Triangles)
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