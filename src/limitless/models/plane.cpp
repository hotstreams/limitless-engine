#include <limitless/models/plane.hpp>

#include "limitless/renderer/renderer_settings.hpp"

using namespace Limitless;

#include <limitless/util/tangent_space.hpp>
#include <limitless/models/mesh_builder.hpp>
#include <limitless/core/vertex_stream/vertex_stream_builder.hpp>

Plane::Plane()
    : Model(
        "plane",
        {[]() {
            /* Plane size (1, 0, 1) centered at (0, 0, 0) */
            std::vector<VertexNormalTangent> vertices = {
                { {0.5f, 0.0f, -0.5f},  { 0.0f, 1.0f, 0.0f }, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), {1.0f, 1.0f} },
                { {0.5f, 0.0f,  0.5f},  { 0.0f, 1.0f, 0.0f }, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), {1.0f, 0.0f} },
                { {-0.5f, 0.0f, 0.5f},  { 0.0f, 1.0f, 0.0f }, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), {0.0f, 0.0f} },
                { {-0.5f, 0.0f, -0.5f}, { 0.0f, 1.0f, 0.0f }, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), {0.0f, 1.0f} }
            };

            std::vector<GLuint> indices = {
                    0, 3, 1,
                    1, 3, 2
            };

            calculateTangentSpaceTriangle(vertices, indices);

            return Mesh::builder()
                    .name("plane_mesh")
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

PlaneQuad::PlaneQuad()
    : Model(
        "plane_quad",
        {[]() {
            /* Plane size (1, 0, 1) centered at (0, 0, 0) */
            std::vector<VertexNormalTangent> vertices = {
                    { {-0.5f, 0.0f, -0.5f}, { 0.0f, 1.0f, 0.0f }, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), {0.0f, 1.0f} },
                    { {0.5f, 0.0f, -0.5f},  { 0.0f, 1.0f, 0.0f }, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), {1.0f, 1.0f} },
                    { {-0.5f, 0.0f, 0.5f},  { 0.0f, 1.0f, 0.0f }, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), {0.0f, 0.0f} },
                    { {0.5f, 0.0f,  0.5f},  { 0.0f, 1.0f, 0.0f }, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), {1.0f, 0.0f} },
            };

            std::vector<GLuint> indices = {
                    0, 1, 3,
                    1, 2, 3
            };

            calculateTangentSpaceTriangle(vertices, indices);

            return Mesh::builder()
                    .name("plane_quad_mesh")
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