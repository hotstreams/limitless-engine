#include <limitless/models/cube.hpp>

#include <limitless/util/tangent_space.hpp>
#include <limitless/models/mesh_builder.hpp>

#include <limitless/core/vertex_stream/vertex_stream_builder.hpp>
#include <limitless/renderer/renderer_settings.hpp>

using namespace Limitless;

Cube::Cube()
    : Model(
        "cube",
        {[]() {
            calculateTangentSpaceTriangle(vertices);

            // Build sequential index buffer so the cube is an indexed stream
            // (required for geometry batching / indirect draw)
            std::vector<GLuint> indices(vertices.size());
            for (GLuint i = 0; i < static_cast<GLuint>(vertices.size()); ++i) {
                indices[i] = i;
            }

            return Mesh::builder()
            .name("cube_mesh")
            .vertex_stream(
                VertexStream::builder()
                    .attribute(0, VertexStream::Attribute::Position, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, position))
                    .attribute(1, VertexStream::Attribute::Normal, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, normal))
                    .attribute(2, VertexStream::Attribute::Tangent, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, tangent))
                    .attribute(3, VertexStream::Attribute::Uv, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, uv))
                    .vertices(vertices)
                    .indices(indices)
                    .usage(VertexStream::Usage::Static)
                    .draw(VertexStream::Draw::Triangles)
                    .batched(RendererSettings::geometry_batching_enabled)
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