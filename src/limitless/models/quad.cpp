#include <limitless/models/quad.hpp>

using namespace Limitless;

#include <limitless/util/tangent_space.hpp>
#include <limitless/models/mesh_builder.hpp>
#include <limitless/core/vertex_stream/vertex_stream_builder.hpp>

Quad::Quad() : ElementaryModel("quad") {
    std::vector<Vertex> vertices = {
            { {-1.0f, 1.0f, 0.0f},  {0.0f, 1.0f} },
            { {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f} },
            { {1.0f, 1.0f, 0.0f},   {1.0f, 1.0f} },
            { {1.0f, -1.0f, 0.0f},  {1.0f, 0.0f} }
    };

    meshes.emplace_back(
        Mesh::builder()
            .name("quad_mesh")
            .vertex_stream(
                VertexStream::builder()
                    .attribute(0, VertexStream::Attribute::Position, sizeof(Vertex), offsetof(Vertex, position))
                    .attribute(1, VertexStream::Attribute::Uv, sizeof(Vertex), offsetof(Vertex, uv))
                    .vertices(vertices)
                    .usage(VertexStream::Usage::Static)
                    .draw(VertexStream::Draw::TriangleStrip)
                    .build()
            )
            .build()
    );

    calculateBoundingBox();
}