#include <limitless/models/mesh_builder.hpp>
#include <limitless/models/sphere.hpp>

using namespace Limitless;

#include <limitless/util/tangent_space.hpp>
#include <limitless/util/math.hpp>
#include <limitless/core/vertex_stream/vertex_stream_builder.hpp>
#include <limitless/models/mesh.hpp>

Sphere::Sphere(glm::uvec2 segment_count) : ElementaryModel("sphere") {
    std::vector<VertexNormalTangent> vertices;
    std::vector<GLuint> indices;

    constexpr auto RADIUS = 1.0f;

    for (uint32_t i = 0; i <= segment_count.y; ++i) {
        glm::vec3 position {};
        glm::vec3 normal {};
        glm::vec2 uv {};

        const auto stack_step = PI / static_cast<float>(segment_count.y);
        const auto stack_angle = PI / 2.0f - static_cast<float>(i) * stack_step;
        const auto xy = RADIUS * glm::cos(stack_angle);
        position.y = RADIUS * glm::sin(stack_angle);

        for (uint32_t j = 0; j <= segment_count.x; ++j) {
            const auto sector_step = 2.0f * PI / static_cast<float>(segment_count.x);
            const auto sector_angle = static_cast<float>(j) * sector_step;

            position.x = xy * glm::cos(sector_angle);
            position.z = xy * glm::sin(sector_angle);

            normal = glm::normalize(position);

            uv.x = static_cast<float>(j) / static_cast<float>(segment_count.x);
            uv.y = static_cast<float>(i) / static_cast<float>(segment_count.y);

            vertices.emplace_back(VertexNormalTangent{position, normal, normal, uv});
        }
    }

    uint64_t k1 {};
    uint64_t k2 {};
    for (uint64_t i = 0; i < segment_count.y; ++i) {
        k1 = i * (segment_count.x + 1);
        k2 = k1 + segment_count.x + 1;

        for (uint64_t j = 0; j < segment_count.x; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.emplace_back(k1 + 1);
                indices.emplace_back(k2);
                indices.emplace_back(k1);
            }

            if (i != (segment_count.y - 1)) {
                indices.emplace_back(k2 + 1);
                indices.emplace_back(k2);
                indices.emplace_back(k1 + 1);
            }
        }
    }

    calculateTangentSpaceTriangle(vertices, indices);

    meshes.emplace_back(
        Mesh::builder()
            .name("sphere_mesh")
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