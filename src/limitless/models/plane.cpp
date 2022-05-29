#include <limitless/models/plane.hpp>

using namespace Limitless;

#include <limitless/util/tangent_space.hpp>
#include <limitless/models/mesh.hpp>
#include <limitless/core/indexed_stream.hpp>

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

    meshes.emplace_back(
        std::make_unique<Mesh>(
            std::make_unique<IndexedVertexStream<VertexNormalTangent>>(std::move(vertices), std::move(indices), VertexStreamUsage::Static, VertexStreamDraw::Triangles),
            "plane_mesh")
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
        std::make_unique<Mesh>(
            std::make_unique<VertexStream<VertexNormalTangent>>(std::move(vertices), VertexStreamUsage::Static, VertexStreamDraw::Patches),
            "planequad_mesh")
    );

    calculateBoundingBox();
}