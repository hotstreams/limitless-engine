#include <limitless/elementary_model.hpp>
#include <limitless/assets.hpp>
#include <limitless/indexed_mesh.hpp>
#include <limitless/util/math.hpp>

using namespace LimitlessEngine;

Line::Line(const glm::vec3 &a, const glm::vec3 &b) {
    std::vector<Vertex> vertices = {{ a,  glm::vec2(0.0f) }, { b,  glm::vec2(0.0f) }};

    auto mesh = std::make_shared<Mesh<Vertex>>(std::move(vertices), "line_mesh", MeshDataType::Static, DrawMode::Lines);
    assets.meshes.add("line_mesh", mesh);
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

    if (!assets.meshes.isExist("quad_mesh")) {
	    assets.meshes.add("quad_mesh", mesh);
    }

    meshes.emplace_back(std::move(mesh));
}

Rectangle::Rectangle() {
    std::vector<VertexNormalTangent> vertices = {
            {{-1.0f, 1.0f, 0.0f}, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f),{0.0f, 1.0f}},
            {{-1.0f, -1.0f, 0.0f}, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f),{0.0f, 0.0f}},
            {{1.0f, 1.0f, 0.0f}, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f),{1.0f, 1.0f}},
            {{1.0f, -1.0f, 0.0f}, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f),{1.0f, 0.0f}}
    };



//	std::vector<Vertex> vertices = {
//			{{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
//			{{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
//			{{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
//			{{0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}
//	};

	auto mesh = std::make_shared<Mesh<VertexNormalTangent>>(std::move(vertices), "rectangle_mesh", MeshDataType::Static, DrawMode::TriangleStrip);

	if (!assets.meshes.isExist("rectangle_mesh")) {
		assets.meshes.add("rectangle_mesh", mesh);
	}

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
    assets.meshes.add("cube_mesh", mesh);
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

    for (uint32_t i=0; i<vertices.size(); i+=3) {
        glm::vec3 & v0 = vertices[i+0].position;
        glm::vec3 & v1 = vertices[i+1].position;
        glm::vec3 & v2 = vertices[i+2].position;

        glm::vec2 & uv0 = vertices[i+0].uv;
        glm::vec2 & uv1 = vertices[i+1].uv;
        glm::vec2 & uv2 = vertices[i+2].uv;

        glm::vec3 deltaPos1 = v1-v0;
        glm::vec3 deltaPos2 = v2-v0;

        glm::vec2 deltaUV1 = uv1-uv0;
        glm::vec2 deltaUV2 = uv2-uv0;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        glm::vec3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;

        vertices[i+0].tangent = tangent;
        vertices[i+1].tangent = tangent;
        vertices[i+2].tangent = tangent;
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
    assets.meshes.add("sphere_mesh", mesh);
    meshes.emplace_back(std::move(mesh));
}
