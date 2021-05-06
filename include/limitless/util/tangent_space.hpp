#pragma once

#include <limitless/core/vertex.hpp>
#include <vector>

namespace Limitless {
    inline void calculateTangentSpace(VertexNormalTangent& vertex0, VertexNormalTangent& vertex1, VertexNormalTangent& vertex2) noexcept {
        const auto& v0 = vertex0.position;
        const auto& v1 = vertex1.position;
        const auto& v2 = vertex2.position;

        const auto& uv0 = vertex0.uv;
        const auto& uv1 = vertex1.uv;
        const auto& uv2 = vertex2.uv;

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

    template<typename I>
    inline void calculateTangentSpaceTriangle(std::vector<VertexNormalTangent>& vertices, const std::vector<I>& indices) {
        for (size_t i = 0; i < indices.size(); i += 3) {
            calculateTangentSpace(vertices.at(indices.at(i)), vertices.at(indices.at(i + 1)), vertices.at(indices.at(i + 2)));
        }
    }

    inline void calculateTangentSpaceTriangle(std::vector<VertexNormalTangent>& vertices) {
        for (size_t i = 0; i < vertices.size(); i += 3) {
            calculateTangentSpace(vertices.at(i), vertices.at(i + 1), vertices.at(i + 2));
        }
    }

    template<typename I>
    inline void calculateTangentSpaceTriangleStrip(std::vector<VertexNormalTangent>& vertices, const std::vector<I>& indices) {
        for (size_t i = 2; i < indices.size(); ++i) {
            const auto i0 = indices.at(i - 2);
            const auto i1 = indices.at(i - 1);
            const auto i2 = indices.at(i);
            calculateTangentSpace(vertices.at(i0), vertices.at(i1), vertices.at(i2));
        }
    }
}