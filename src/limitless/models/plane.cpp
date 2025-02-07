#include <limitless/models/plane.hpp>

using namespace Limitless;

#include <limitless/util/tangent_space.hpp>
#include <limitless/models/mesh_builder.hpp>
#include <limitless/core/vertex_stream/vertex_stream_builder.hpp>

Plane::Plane() : ElementaryModel("plane") {
    /* Plane size (1, 0, 1) centered at (0, 0, 0) */
    std::vector<VertexNormalTangent> vertices = {
            { {0.5f, 0.0f, -0.5f},  { 0.0f, 1.0f, 0.0f }, glm::vec3{0.0f}, {1.0f, 1.0f} },
            { {0.5f, 0.0f,  0.5f},  { 0.0f, 1.0f, 0.0f }, glm::vec3{0.0f}, {1.0f, 0.0f} },
            { {-0.5f, 0.0f, 0.5f},  { 0.0f, 1.0f, 0.0f }, glm::vec3{0.0f}, {0.0f, 0.0f} },
            { {-0.5f, 0.0f, -0.5f}, { 0.0f, 1.0f, 0.0f }, glm::vec3{0.0f}, {0.0f, 1.0f} }
    };

    std::vector<GLuint> indices = {
            0, 3, 1,
            1, 3, 2
    };

    calculateTangentSpaceTriangle(vertices, indices);

    meshes.emplace_back(
        Mesh::builder()
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
                    .build()
            )
            .build()
    );

    calculateBoundingBox();
}

PlaneQuad::PlaneQuad() : ElementaryModel("planequad") {
    /* Plane size (1, 0, 1) centered at (0, 0, 0) */
    std::vector<VertexNormalTangent> vertices = {
            { {-0.5f, 0.0f, -0.5f}, { 0.0f, 1.0f, 0.0f }, glm::vec3{0.0f}, {0.0f, 1.0f} },
            { {0.5f, 0.0f, -0.5f},  { 0.0f, 1.0f, 0.0f }, glm::vec3{0.0f}, {1.0f, 1.0f} },
            { {-0.5f, 0.0f, 0.5f},  { 0.0f, 1.0f, 0.0f }, glm::vec3{0.0f}, {0.0f, 0.0f} },
            { {0.5f, 0.0f,  0.5f},  { 0.0f, 1.0f, 0.0f }, glm::vec3{0.0f}, {1.0f, 0.0f} },
    };

    std::vector<GLuint> indices = {
            0, 1, 3,
            1, 2, 3
    };

    calculateTangentSpaceTriangle(vertices, indices);

    meshes.emplace_back(
        Mesh::builder()
            .name("planequad_mesh")
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
            .build()
    );

    calculateBoundingBox();
}