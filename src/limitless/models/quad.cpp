#include <limitless/models/quad.hpp>

using namespace Limitless;

#include <limitless/util/tangent_space.hpp>
#include <limitless/models/mesh.hpp>

Quad::Quad() {
    std::vector<Vertex> vertices = {
            { {-1.0f, 1.0f, 0.0f},  {0.0f, 1.0f} },
            { {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f} },
            { {1.0f, 1.0f, 0.0f},   {1.0f, 1.0f} },
            { {1.0f, -1.0f, 0.0f},  {1.0f, 0.0f} }
    };

    meshes.emplace_back(new Mesh(std::move(vertices), "quad_mesh", MeshDataType::Static, DrawMode::TriangleStrip));
    calculateBoundingBox();
}