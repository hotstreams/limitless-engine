#include <elementary_model.hpp>
#include <assets.hpp>
#include <indexed_mesh.hpp>

using namespace GraphicsEngine;

Line::Line(const glm::vec3 &a, const glm::vec3 &b) {
    std::vector<Vertex> vertices = {{ a,  glm::vec2(0.0f) }, { b,  glm::vec2(0.0f) }};

    auto mesh = std::make_shared<Mesh<Vertex>>(std::move(vertices), "line_mesh", MeshDataType::Static, DrawMode::Lines);
    meshes.emplace_back(std::move(mesh));
}

Quad::Quad() {
    std::vector<Vertex> vertices = {
        {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}
    };

    auto mesh = std::make_shared<Mesh<Vertex>>(std::move(vertices), "quad_mesh", MeshDataType::Static, DrawMode::TriangleStrip);
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
                    std::cos(x_seg * 2.0f * M_PI) * std::sin(y_seg * M_PI),
                    std::cos(y_seg * M_PI),
                    std::sin(x_seg * 2.0f * M_PI) * std::sin(y_seg * M_PI)
            };

            vertices.emplace_back(VertexNormalTangent{pos, pos, pos, glm::vec2{x_seg, y_seg}});
        }
    }

    bool oddRow = false;
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

    auto mesh = std::make_shared<IndexedMesh<VertexNormalTangent, GLuint>>(std::move(vertices), std::move(indices), "sphere_mesh", MeshDataType::Static, DrawMode::TriangleStrip);
    meshes.emplace_back(std::move(mesh));
}
