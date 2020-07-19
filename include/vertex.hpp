#pragma once

#include <glm/glm.hpp>

namespace GraphicsEngine {
    // simplest vertex attribute
    struct Vertex {
        glm::vec3 position;
        glm::vec2 uv;
    };

    // vertex + normal
    struct VertexNormal {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    // vertex + normal + tangent space (normal mapping)
    struct VertexNormalTangent {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec2 uv;
    };

    // packed glm::vec3 into GL_INT_2_10_10_10
    union PackedFloat3 {
        int i32;
        struct {
            int x : 10;
            int y : 10;
            int z : 10;
            int w : 2;
        } i32f3;
    };

    struct VertexPacked {
        glm::vec3 position;
        uint32_t uv;
    };

    struct VertexPackedNormal {
        glm::vec3 position;
        PackedFloat3 normal;
        uint32_t uv;
    };

    struct VertexPackedNormalTangent {
        glm::vec3 position;
        PackedFloat3 normal;
        PackedFloat3 tangent;
        uint32_t uv;
    };
}