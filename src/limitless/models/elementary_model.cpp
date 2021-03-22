#include <limitless/models/elementary_model.hpp>
#include <limitless/assets.hpp>
#include <limitless/models/indexed_mesh.hpp>
#include <limitless/util/math.hpp>

using namespace LimitlessEngine;

namespace {
    void calculateTangentSpace(VertexNormalTangent& vertex0, VertexNormalTangent& vertex1, VertexNormalTangent& vertex2) noexcept {
        const auto& v0 = vertex0.position;
        const auto& v1 = vertex1.position;
        const auto& v2 = vertex2.position;

        auto& uv0 = vertex0.uv;
        auto& uv1 = vertex1.uv;
        auto& uv2 = vertex2.uv;

        const auto deltaPos1 = v1 - v0;
        const auto deltaPos2 = v2 - v0;

        const auto deltaUV1 = uv1 - uv0;
        const auto deltaUV2 = uv2 - uv0;

        const auto r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        const auto tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;

        vertex0.tangent = tangent;
        vertex1.tangent = tangent;
        vertex2.tangent = tangent;
    }

    void calculateTangentSpaceTriangle(std::vector<VertexNormalTangent>& vertices, const std::vector<GLuint>& indices) {
        for (size_t i = 0; i < indices.size(); i += 3) {
            calculateTangentSpace(vertices.at(indices.at(i)), vertices.at(indices.at(i + 1)), vertices.at(indices.at(i + 2)));
        }
    }

    void calculateTangentSpaceTriangleStrip(std::vector<VertexNormalTangent>& vertices, const std::vector<GLuint>& indices) {
        for (size_t i = 2; i < indices.size(); ++i) {
            const auto i0 = indices.at(i - 2);
            const auto i1 = indices.at(i - 1);
            const auto i2 = indices.at(i);
            calculateTangentSpace(vertices.at(i0), vertices.at(i1), vertices.at(i2));
        }
    }
}

Line::Line(const glm::vec3 &a, const glm::vec3 &b) {
    std::vector<Vertex> vertices = {{ a,  glm::vec2(0.0f) }, { b,  glm::vec2(0.0f) }};

    auto mesh = std::make_shared<Mesh<Vertex>>(std::move(vertices), "line_mesh", MeshDataType::Static, DrawMode::Lines);
    meshes.emplace_back(std::move(mesh));
}

Quad::Quad() {
    std::vector<Vertex> vertices = {
        { {-1.0f, 1.0f, 0.0f},  {0.0f, 1.0f} },
        { {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f} },
        { {1.0f, 1.0f, 0.0f},   {1.0f, 1.0f} },
        { {1.0f, -1.0f, 0.0f},  {1.0f, 0.0f} }
    };

    auto mesh = std::make_shared<Mesh<Vertex>>(std::move(vertices), "quad_mesh", MeshDataType::Static, DrawMode::TriangleStrip);
    meshes.emplace_back(std::move(mesh));
}

Plane::Plane() {
    /* Plane size (1, 0, 1) centered at (0, 0, 0) */
    std::vector<VertexNormalTangent> vertices = {
        { {0.5f, 0.0f, -0.5f},  { 0.0f, 1.0f, 0.0f }, glm::vec3{0.0f}, {1.0f, 1.0f} },
        { {0.5f, 0.0f,  0.5f},  { 0.0f, 1.0f, 0.0f }, glm::vec3{0.0f}, {1.0f, 0.0f} },
        { {-0.5f, 0.0f, 0.5f},  { 0.0f, 1.0f, 0.0f }, glm::vec3{0.0f}, {0.0f, 0.0f} },
        { {-0.5f, 0.0f, -0.5f}, { 0.0f, 1.0f, 0.0f }, glm::vec3{0.0f}, {0.0f, 1.0f} }
    };

    std::vector<GLuint> indices = {
        0, 1, 3,
        1, 2, 3
    };

    calculateTangentSpaceTriangle(vertices, indices);

    auto mesh = std::make_shared<IndexedMesh<VertexNormalTangent, GLuint>>(std::move(vertices), std::move(indices), "plane_mesh", MeshDataType::Static, DrawMode::Triangles);

    meshes.emplace_back(std::move(mesh));
}

Cube::Cube() {
    std::vector<Vertex> vertices = {
            // back face
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}}, // bottom-left
            {{0.5f, -0.5f, -0.5f},  {1.0f, 0.0f}}, // bottom-right
            {{0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}}, // top-right
            {{0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}}, // top-right
            {{ -0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}}, // top-left
            {{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f}}, // bottom-left
            // front face
            {{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}}, // bottom-left
            {{ 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f}}, // top-right
            {{0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}}, // bottom-right
            {{0.5f,  0.5f,  0.5f},  {1.0f, 1.0f}}, // top-right
            {{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}}, // bottom-left
            {{-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f}}, // top-left
            // left face
            {{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}}, // top-right
            {{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}}, // bottom-left
            {{-0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}}, // top-left
            {{ -0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}}, // bottom-left
            {{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}}, // top-right
            {{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}}, // bottom-right
            // right face
            {{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}}, // top-left
            {{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}}, // top-right
            {{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}}, // bottom-right
            {{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}}, // bottom-right
            {{  0.5f, -0.5f,  0.5f}, { 0.0f, 0.0f}}, // bottom-left
            {{  0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f}}, // top-left
            // bottom face
            {{ -0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}}, // top-right
            {{  0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}}, // bottom-left
            {{  0.5f, -0.5f, -0.5f},  {1.0f, 1.0f}}, // top-left
            {{ 0.5f, -0.5f,  0.5f}, { 1.0f, 0.0f}}, // bottom-left
            {{ -0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f}}, // top-right
            {{ -0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}}, // bottom-right
            // top face
            {{ -0.5f,  0.5f, -0.5f},  {0.0f, 1.0f}}, // top-left
            {{ 0.5f,  0.5f, -0.5f}, { 1.0f, 1.0f}}, // top-right
            {{ 0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f}}, // bottom-right
            {{ 0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f}}, // bottom-right
            {{ -0.5f,  0.5f,  0.5f}, { 0.0f, 0.0f}}, // bottom-left
            {{ -0.5f,  0.5f, -0.5f}, { 0.0f, 1.0f}}  // top-left
    };

    auto mesh = std::make_shared<Mesh<Vertex>>(std::move(vertices), "cube_mesh", MeshDataType::Static, DrawMode::Triangles);
    meshes.emplace_back(mesh);
}

Sphere::Sphere(uint32_t x_segments, uint32_t y_segments) {
    std::vector<VertexNormalTangent> vertices;
    std::vector<GLuint> indices;

    for (uint32_t y = 0; y <= y_segments; ++y) {
        for (uint32_t x = 0; x <= x_segments; ++x) {
            auto x_seg = static_cast<float>(x) / static_cast<float>(x_segments);
            auto y_seg = static_cast<float>(y) / static_cast<float>(y_segments);

            glm::vec3 pos = {
                std::cos(x_seg * 2.0f * pi) * std::sin(y_seg * pi),
                std::cos(y_seg * M_PI),
                std::sin(x_seg * 2.0f * pi) * std::sin(y_seg * pi)
            };

            vertices.emplace_back(VertexNormalTangent{pos, pos, pos, glm::vec2{x_seg, y_seg}});
        }
    }

    bool oddRow {};
    for (uint32_t y = 0; y < y_segments; ++y) {
        if (!oddRow) {
            for (uint32_t x = 0; x <= x_segments; ++x) {
                indices.push_back(y * (x_segments + 1) + x);
                indices.push_back((y + 1) * (x_segments + 1) + x);
            }
        } else {
            for (int x = x_segments; x >= 0; --x) {
                indices.emplace_back((y + 1) * (x_segments + 1) + x);
                indices.emplace_back(y * (x_segments + 1) + x);
            }
        }
        oddRow = !oddRow;
    }

    calculateTangentSpaceTriangleStrip(vertices, indices);

    auto mesh = std::make_shared<IndexedMesh<VertexNormalTangent, GLuint>>(std::move(vertices), std::move(indices), "sphere_mesh", MeshDataType::Static, DrawMode::TriangleStrip);
    meshes.emplace_back(std::move(mesh));
}
