#include <limitless/models/cube.hpp>

#include <limitless/util/tangent_space.hpp>
#include <limitless/models/mesh_builder.hpp>

#include <limitless/core/vertex_stream/vertex_stream_builder.hpp>

using namespace Limitless;

Cube::Cube() : ElementaryModel("cube") {
    //TODO: fix face order
    std::vector<VertexNormalTangent> vertices = {
            // back face
            {{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, glm::vec3{0.0f}, {0.0f, 0.0f}}, // bottom-left
            {{0.5f, -0.5f, -0.5f},   {0.0f, 0.0f, -1.0f}, glm::vec3{0.0f}, {1.0f, 0.0f}}, // bottom-right
            {{0.5f,  0.5f, -0.5f},   {0.0f, 0.0f, -1.0f}, glm::vec3{0.0f}, {1.0f, 1.0f}}, // top-right
            {{0.5f,  0.5f, -0.5f},   {0.0f, 0.0f, -1.0f}, glm::vec3{0.0f}, {1.0f, 1.0f}}, // top-right
            {{ -0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, glm::vec3{0.0f}, {0.0f, 1.0f}}, // top-left
            {{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, glm::vec3{0.0f}, {0.0f, 0.0f}}, // bottom-left
            // front face
            {{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 0.0f}}, // bottom-left
            {{ 0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 1.0f}}, // top-right
            {{0.5f, -0.5f,  0.5f},   {0.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 0.0f}}, // bottom-right
            {{0.5f,  0.5f,  0.5f},   {0.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 1.0f}}, // top-right
            {{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 0.0f}}, // bottom-left
            {{-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 1.0f}}, // top-left
            // left face
            {{-0.5f,  0.5f,  0.5f},  {-1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 0.0f}}, // top-right
            {{-0.5f, -0.5f, -0.5f},  {-1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 1.0f}}, // bottom-left
            {{-0.5f,  0.5f, -0.5f},  {-1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 1.0f}}, // top-left
            {{ -0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 1.0f}}, // bottom-left
            {{-0.5f,  0.5f,  0.5f},  {-1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 0.0f}}, // top-right
            {{-0.5f, -0.5f,  0.5f},  {-1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 0.0f}}, // bottom-right
            // right face
            {{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 0.0f}}, // top-left
            {{ 0.5f,  0.5f, -0.5f},  {1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 1.0f}}, // top-right
            {{ 0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 1.0f}}, // bottom-right
            {{ 0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 1.0f}}, // bottom-right
            {{  0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, { 0.0f, 0.0f}}, // bottom-left
            {{  0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, { 1.0f, 0.0f}}, // top-left
            // bottom face
            {{ -0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 1.0f}}, // top-right
            {{  0.5f, -0.5f,  0.5f},  {0.0f, -1.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 0.0f}}, // bottom-left
            {{  0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 1.0f}}, // top-left
            {{ 0.5f, -0.5f,  0.5f},   {0.0f, -1.0f, 1.0f}, glm::vec3{0.0f}, { 1.0f, 0.0f}}, // bottom-left
            {{ -0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 1.0f}, glm::vec3{0.0f}, { 0.0f, 1.0f}}, // top-right
            {{ -0.5f, -0.5f,  0.5f},  {0.0f, -1.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 0.0f}}, // bottom-right
            // top face
            {{ -0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 1.0f}}, // top-left
            {{ 0.5f,  0.5f, -0.5f},   {0.0f, 1.0f, 1.0f}, glm::vec3{0.0f}, { 1.0f, 1.0f}}, // top-right
            {{ 0.5f,  0.5f,  0.5f},   {0.0f, 1.0f, 1.0f}, glm::vec3{0.0f}, { 1.0f, 0.0f}}, // bottom-right
            {{ 0.5f,  0.5f,  0.5f},   {0.0f, 1.0f, 1.0f}, glm::vec3{0.0f}, { 1.0f, 0.0f}}, // bottom-right
            {{ -0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 1.0f}, glm::vec3{0.0f}, { 0.0f, 0.0f}}, // bottom-left
            {{ -0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 1.0f}, glm::vec3{0.0f}, { 0.0f, 1.0f}}  // top-left
    };

    calculateTangentSpaceTriangle(vertices);

    meshes.emplace_back(
        Mesh::builder()
            .name("cube_mesh")
            .vertex_stream(
                VertexStream::builder()
                    .attribute(0, VertexStream::Attribute::Position, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, position))
                    .attribute(1, VertexStream::Attribute::Normal, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, normal))
                    .attribute(2, VertexStream::Attribute::Tangent, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, tangent))
                    .attribute(3, VertexStream::Attribute::Uv, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, uv))
                    .vertices(vertices)
                    .usage(VertexStream::Usage::Static)
                    .draw(VertexStream::Draw::Triangles)
                    .build()
            )
            .build()
    );

    calculateBoundingBox();
}