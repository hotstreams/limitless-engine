#include <limitless/models/plane.hpp>

using namespace Limitless;

#include <limitless/util/tangent_space.hpp>
#include <limitless/models/indexed_mesh.hpp>

Plane::Plane() : ElementaryModel("plane") {
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

    meshes.emplace_back(new IndexedMesh(std::move(vertices), std::move(indices), "plane", MeshDataType::Static, DrawMode::Triangles));
    calculateBoundingBox();
}