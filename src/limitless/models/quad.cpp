#include <limitless/models/quad.hpp>

using namespace Limitless;

#include <limitless/util/tangent_space.hpp>
#include <limitless/models/mesh.hpp>

Quad::Quad() : ElementaryModel("quad") {
    std::vector<Vertex> vertices = {
            { {-1.0f, 1.0f, 0.0f},  {0.0f, 1.0f} },
            { {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f} },
            { {1.0f, 1.0f, 0.0f},   {1.0f, 1.0f} },
            { {1.0f, -1.0f, 0.0f},  {1.0f, 0.0f} }
    };

    meshes.emplace_back(
        std::make_unique<Mesh>(
            std::make_unique<VertexStream<Vertex>>(std::move(vertices), VertexStreamUsage::Static, VertexStreamDraw::TriangleStrip),
            "quad_mesh")
    );

    calculateBoundingBox();
}