#include <limitless/models/cylinder.hpp>

#include <limitless/util/tangent_space.hpp>
#include <limitless/models/mesh.hpp>
#include <limitless/core/indexed_stream.hpp>

#include <cmath>

using namespace Limitless;

Cylinder::Cylinder() : ElementaryModel("cylinder") {
    const auto unit = generateUnit();
    const auto normals = generateNormals();

    auto vertices = generate(unit, normals);
    auto indices = generateIndices();

    calculateTangentSpaceTriangle(vertices, indices);

    meshes.emplace_back(
        std::make_unique<Mesh>(
            std::make_unique<IndexedVertexStream<VertexNormalTangent>>(std::move(vertices),
                                                                       std::move(indices),
                                                                       VertexStreamUsage::Static,
                                                                       VertexStreamDraw::Triangles),
            "cylinder")
    );

//    calculateBoundingBox();
}

Cylinder::Cylinder(float base_radius, float top_radius, float height)
    : ElementaryModel("cylinder")
    , base_radius {base_radius}
    , top_radius {top_radius}
    , height {height} {

    const auto unit = generateUnit();
    const auto normals = generateNormals();

    auto vertices = generate(unit, normals);
    auto indices = generateIndices();

    calculateTangentSpaceTriangle(vertices, indices);

    meshes.emplace_back(
            std::make_unique<Mesh>(
                    std::make_unique<IndexedVertexStream<VertexNormalTangent>>(std::move(vertices),
                                                                               std::move(indices),
                                                                               VertexStreamUsage::Static,
                                                                               VertexStreamDraw::Triangles),
                    "cylinder")
    );

//    calculateBoundingBox();
}

std::vector<glm::vec3> Cylinder::generateNormals() const {
    std::vector<glm::vec3> normals;
    normals.reserve(sector_count);

    const auto sector_step = 2.0f * M_PI / sector_count;
    const auto zAngle = std::atan2(base_radius - top_radius, height);
    const auto x0 = cos(zAngle);
    const auto z0 = sin(zAngle);

    for (uint32_t i = 0; i <= sector_count; ++i) {
        const auto sector_angle = i * sector_step;
        normals.emplace_back(cos(sector_angle) * x0 - sin(sector_angle) * z0, 0.0f, sin(sector_angle) * x0 + cos(sector_angle) * z0);
    }

    return normals;
}

std::vector<glm::vec3> Cylinder::generateUnit() const {
    std::vector<glm::vec3> unit;
    unit.reserve(sector_count);

    const auto sector_step = 2.0f * M_PI / sector_count;

    for (uint32_t i = 0; i <= sector_count; ++i) {
        const auto sector_angle = i * sector_step;
        unit.emplace_back(cos(sector_angle), 0.0f, sin(sector_angle));
    }

    return unit;
}

std::vector<VertexNormalTangent> Cylinder::generate(const std::vector<glm::vec3>& unit, const std::vector<glm::vec3>& normals) {
    std::vector<VertexNormalTangent> vertices;

    glm::vec3 position {};

    for (uint32_t i = 0; i <= stack_count; ++i) {
        position.y = static_cast<float>(i) / static_cast<float>(stack_count) * height;
        const auto radius = base_radius + static_cast<float>(i) / static_cast<float>(stack_count) * (top_radius - base_radius);
        const auto t = 1.0f - static_cast<float>(i) / static_cast<float>(stack_count);

        for (uint32_t j = 0; j <= sector_count; ++j) {
            position.x = unit[j].x;
            position.z = unit[j].z;

            position.x *= radius;
            position.z *= radius;

            vertices.emplace_back(VertexNormalTangent{position, normals[j], glm::vec3{0.0f}, glm::vec2(static_cast<float>(j) / static_cast<float>(sector_count), t)});
        }
    }

    base_vertex_index = vertices.size();

    position.y = 0.0f;

    vertices.emplace_back(VertexNormalTangent{glm::vec3{0.0f, position.y, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f}, glm::vec3{0.0f}, glm::vec2(0.5f)});

    for (uint32_t i = 0; i < sector_count; ++i) {
        position.x = unit[i].x;
        position.z = unit[i].z;

        vertices.emplace_back(VertexNormalTangent{glm::vec3{position.x * base_radius,  position.y, position.z * base_radius},
                                                  glm::vec3{0.0f, -1.0f, 0.0f},
                                                  glm::vec3{0.0f},
                                                  glm::vec2(-position.x * 0.5f + 0.5f, -position.y * 0.5f + 0.5f)});
    }

    top_vertex_index = vertices.size();

    position.y = height;

    vertices.emplace_back(VertexNormalTangent{glm::vec3{0.0f, position.y, 0.0f},
                                              glm::vec3{0.0f, 1.0f, 0.0f},
                                              glm::vec3{0.0f},
                                              glm::vec2(0.5f)});


    for (uint32_t i = 0; i < sector_count; ++i) {
        position.x = unit[i].x;
        position.z = unit[i].z;

        vertices.emplace_back(VertexNormalTangent{glm::vec3{position.x * top_radius, position.y, position.z * top_radius},
                                                  glm::vec3{0.0f, 1.0f, 0.0f},
                                                  glm::vec3{0.0f},
                                                  glm::vec2(position.x * 0.5f + 0.5f, -position.y * 0.5f + 0.5f)});
    }

    return vertices;
}

std::vector<uint32_t> Cylinder::generateIndices() {
    std::vector<uint32_t> indices;

    uint32_t k1 {};
    uint32_t k2 {};

    for (uint32_t i = 0; i < stack_count; ++i) {
        k1 = i * (sector_count + 1);
        k2 = k1 + sector_count + 1;

        for (uint32_t j = 0; j < sector_count; ++j, ++k1, ++k2) {
            indices.emplace_back(k1);
            indices.emplace_back(k1 + 1);
            indices.emplace_back(k2);

            indices.emplace_back(k2);
            indices.emplace_back(k1 + 1);
            indices.emplace_back(k2 + 1);
        }
    }

    for (uint32_t i = 0, k = base_vertex_index + 1; i < sector_count; ++i, ++k) {
        if (i < (sector_count - 1)) {
            indices.emplace_back(base_vertex_index);
            indices.emplace_back(k + 1);
            indices.emplace_back(k);
        } else {
            indices.emplace_back(base_vertex_index);
            indices.emplace_back(base_vertex_index + 1);
            indices.emplace_back(k);
        }
    }

    for (uint32_t i = 0, k = top_vertex_index + 1; i < sector_count; ++i, ++k) {
        if (i < (sector_count - 1)) {
            indices.emplace_back(top_vertex_index);
            indices.emplace_back(k);
            indices.emplace_back(k + 1);
        }
        else {
            indices.emplace_back(top_vertex_index);
            indices.emplace_back(k);
            indices.emplace_back(top_vertex_index + 1);
        }
    }

    return indices;
}
