#pragma once

#include <limitless/models/model.hpp>

#include "limitless/core/vertex.hpp"

namespace Limitless {
    class Cube : public Model {
    private:
        static inline std::vector<VertexNormalTangent> vertices = {
            //TODO: fix face order
            // back face
            {{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, glm::vec3{0.0f}, {0.0f, 0.0f}}, // bottom-left
            {{0.5f, -0.5f, -0.5f},   {0.0f, 0.0f, -1.0f}, glm::vec3{0.0f}, {1.0f, 0.0f}}, // bottom-right
            {{0.5f,  0.5f, -0.5f},   {0.0f, 0.0f, -1.0f}, glm::vec3{0.0f}, {1.0f, 1.0f}}, // top-right
            {{0.5f,  0.5f, -0.5f},   {0.0f, 0.0f, -1.0f}, glm::vec3{0.0f}, {1.0f, 1.0f}}, // top-right
            {{ -0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, glm::vec3{0.0f}, {0.0f, 1.0f}}, // top-left
            {{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, glm::vec3{0.0f}, {0.0f, 0.0f}}, // bottom-left
            // front face
            {{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 0.0f}}, // bottom-left
            {{ 0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 1.0f}}, // top-right
            {{0.5f, -0.5f,  0.5f},   {0.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 0.0f}}, // bottom-right
            {{0.5f,  0.5f,  0.5f},   {0.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 1.0f}}, // top-right
            {{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 0.0f}}, // bottom-left
            {{-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 1.0f}}, // top-left
            // left face
            {{-0.5f,  0.5f,  0.5f},  {-1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 0.0f}}, // top-right
            {{-0.5f, -0.5f, -0.5f},  {-1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 1.0f}}, // bottom-left
            {{-0.5f,  0.5f, -0.5f},  {-1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 1.0f}}, // top-left
            {{ -0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 1.0f}}, // bottom-left
            {{-0.5f,  0.5f,  0.5f},  {-1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 0.0f}}, // top-right
            {{-0.5f, -0.5f,  0.5f},  {-1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 0.0f}}, // bottom-right
            // right face
            {{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 0.0f}}, // top-left
            {{ 0.5f,  0.5f, -0.5f},  {1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 1.0f}}, // top-right
            {{ 0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 1.0f}}, // bottom-right
            {{ 0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 1.0f}}, // bottom-right
            {{  0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, { 0.0f, 0.0f}}, // bottom-left
            {{  0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 1.0f}, glm::vec3{0.0f}, { 1.0f, 0.0f}}, // top-left
            // bottom face
            {{ -0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 1.0f}}, // top-right
            {{  0.5f, -0.5f,  0.5f},  {0.0f, -1.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 0.0f}}, // bottom-left
            {{  0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 1.0f}, glm::vec3{0.0f}, {1.0f, 1.0f}}, // top-left
            {{ 0.5f, -0.5f,  0.5f},   {0.0f, -1.0f, 1.0f}, glm::vec3{0.0f}, { 1.0f, 0.0f}}, // bottom-left
            {{ -0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 1.0f}, glm::vec3{0.0f}, { 0.0f, 1.0f}}, // top-right
            {{ -0.5f, -0.5f,  0.5f},  {0.0f, -1.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 0.0f}}, // bottom-right
            // top face
            {{ -0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 1.0f}, glm::vec3{0.0f}, {0.0f, 1.0f}}, // top-left
            {{ 0.5f,  0.5f, -0.5f},   {0.0f, 1.0f, 1.0f}, glm::vec3{0.0f}, { 1.0f, 1.0f}}, // top-right
            {{ 0.5f,  0.5f,  0.5f},   {0.0f, 1.0f, 1.0f}, glm::vec3{0.0f}, { 1.0f, 0.0f}}, // bottom-right
            {{ 0.5f,  0.5f,  0.5f},   {0.0f, 1.0f, 1.0f}, glm::vec3{0.0f}, { 1.0f, 0.0f}}, // bottom-right
            {{ -0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 1.0f}, glm::vec3{0.0f}, { 0.0f, 0.0f}}, // bottom-left
            {{ -0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 1.0f}, glm::vec3{0.0f}, { 0.0f, 1.0f}}  // top-left
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
