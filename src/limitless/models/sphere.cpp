#include <limitless/models/sphere.hpp>

using namespace Limitless;

#include <limitless/util/tangent_space.hpp>
#include <limitless/util/math.hpp>
#include <limitless/models/indexed_mesh.hpp>

Sphere::Sphere(uint32_t x_segments, uint32_t y_segments) {
    std::vector<VertexNormalTangent> vertices;
    std::vector<GLuint> indices;

    for (uint32_t y = 0; y <= y_segments; ++y) {
        for (uint32_t x = 0; x <= x_segments; ++x) {
            auto x_seg = static_cast<float>(x) / static_cast<float>(x_segments);
            auto y_seg = static_cast<float>(y) / static_cast<float>(y_segments);

            glm::vec3 pos = {
                    std::sin(x_seg * 2.0f * pi) * std::sin(y_seg * pi),
                    std::cos(y_seg * pi),
                    std::cos(x_seg * 2.0f * pi) * std::sin(y_seg * pi)
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

    meshes.emplace_back(new IndexedMesh(std::move(vertices), std::move(indices), "sphere_mesh", MeshDataType::Static, DrawMode::TriangleStrip));
    calculateBoundingBox();
}