#include <limitless/models/cone.hpp>

#include <limitless/util/tangent_space.hpp>
#include <limitless/models/mesh.hpp>

using namespace Limitless;

Cone::Cone() : ElementaryModel("cone") {
    std::vector<VertexNormalTangent> vertices;

    calculateTangentSpaceTriangle(vertices);

    meshes.emplace_back(
        std::make_unique<Mesh>(
                std::make_unique<VertexStream<VertexNormalTangent>>(std::move(vertices), VertexStreamUsage::Static, VertexStreamDraw::Triangles),
                "cube_mesh")
    );

    calculateBoundingBox();
}