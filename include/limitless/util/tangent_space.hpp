#pragma once

#include <limitless/core/vertex.hpp>
#include <vector>

namespace Limitless {
    template<typename Vertex>
    inline void calculateTangentSpace(Vertex& vertex0, Vertex& vertex1, Vertex& vertex2) noexcept {
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

    template<typename Vertex, typename I>
    inline void calculateTangentSpaceTriangle(std::vector<Vertex>& vertices, const std::vector<I>& indices) {
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
    
    template<typename Vertex, typename I>
    inline void calculateTangentSpaceSmooth(std::vector<Vertex>& vertices, const std::vector<I>& indices) {
        for (auto& vertex : vertices) {
            vertex.tangent = glm::vec3(0.0f);
        }
        
        // Accumulate tangents from all triangles
        for (size_t i = 0; i < indices.size(); i += 3) {
            const auto i0 = indices[i];
            const auto i1 = indices[i + 1];
            const auto i2 = indices[i + 2];
            
            auto& v0 = vertices[i0];
            auto& v1 = vertices[i1];
            auto& v2 = vertices[i2];
            
            const auto deltaPos1 = v1.position - v0.position;
            const auto deltaPos2 = v2.position - v0.position;
            
            const auto deltaUV1 = v1.uv - v0.uv;
            const auto deltaUV2 = v2.uv - v0.uv;
            
            const float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x + 1e-8f);
            const auto tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
            
            // Accumulate (add) instead of assign
            v0.tangent += tangent;
            v1.tangent += tangent;
            v2.tangent += tangent;
        }
        
        // Normalize all accumulated tangents
        for (auto& vertex : vertices) {
            float len = glm::length(vertex.tangent);
            if (len > 1e-6f) {
                vertex.tangent /= len;
            } else {
                vertex.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
            }
        }
    }
}