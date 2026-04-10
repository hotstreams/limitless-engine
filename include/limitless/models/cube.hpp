#pragma once

#include <limitless/models/model.hpp>

#include "limitless/core/vertex.hpp"

namespace Limitless {
    class Cube : public Model {
    private:
        // Cube vertices with correct normals and CCW winding order (front-facing when viewed from outside)
        static inline std::vector<VertexNormalTangent> vertices = {
            // back face (-Z normal, CCW when viewed from -Z)
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, glm::vec4(0.0f), {1.0f, 0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, glm::vec4(0.0f), {0.0f, 1.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, glm::vec4(0.0f), {0.0f, 0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, glm::vec4(0.0f), {0.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, glm::vec4(0.0f), {1.0f, 0.0f}},
            {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, glm::vec4(0.0f), {1.0f, 1.0f}},
            // front face (+Z normal, CCW when viewed from +Z)
            {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, glm::vec4(0.0f), {0.0f, 0.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, glm::vec4(0.0f), {1.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, glm::vec4(0.0f), {1.0f, 1.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, glm::vec4(0.0f), {1.0f, 1.0f}},
            {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, glm::vec4(0.0f), {0.0f, 1.0f}},
            {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, glm::vec4(0.0f), {0.0f, 0.0f}},
            // left face (-X normal, CCW when viewed from -X)
            {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, glm::vec4(0.0f), {1.0f, 1.0f}},
            {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, glm::vec4(0.0f), {0.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, glm::vec4(0.0f), {0.0f, 0.0f}},
            {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, glm::vec4(0.0f), {0.0f, 0.0f}},
            {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, glm::vec4(0.0f), {1.0f, 0.0f}},
            {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, glm::vec4(0.0f), {1.0f, 1.0f}},
            // right face (+X normal, CCW when viewed from +X)
            {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, glm::vec4(0.0f), {0.0f, 1.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, glm::vec4(0.0f), {1.0f, 0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, glm::vec4(0.0f), {1.0f, 1.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, glm::vec4(0.0f), {1.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, glm::vec4(0.0f), {0.0f, 1.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, glm::vec4(0.0f), {0.0f, 0.0f}},
            // bottom face (-Y normal, CCW when viewed from -Y)
            {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, glm::vec4(0.0f), {0.0f, 0.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, glm::vec4(0.0f), {1.0f, 0.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, glm::vec4(0.0f), {1.0f, 1.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, glm::vec4(0.0f), {1.0f, 1.0f}},
            {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, glm::vec4(0.0f), {0.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, glm::vec4(0.0f), {0.0f, 0.0f}},
            // top face (+Y normal, CCW when viewed from +Y)
            {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, glm::vec4(0.0f), {0.0f, 1.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, glm::vec4(0.0f), {1.0f, 0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, glm::vec4(0.0f), {1.0f, 1.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, glm::vec4(0.0f), {1.0f, 0.0f}},
            {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, glm::vec4(0.0f), {0.0f, 1.0f}},
            {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, glm::vec4(0.0f), {0.0f, 0.0f}}
    };
    public:
        Cube();
        ~Cube() override = default;

        Cube(const Cube&) = delete;
        Cube& operator=(const Cube&) = delete;

        Cube(Cube&&) noexcept = default;
        Cube& operator=(Cube&&) noexcept = default;
    };
}
