#pragma once

#include <mesh.hpp>
#include <material.hpp>

namespace GraphicsEngine {
    class AbstractModel {
    protected:
        std::vector<std::shared_ptr<AbstractMesh>> meshes;
    public:
        AbstractModel() = default;
        virtual ~AbstractModel() = default;

        [[nodiscard]] const auto& getMeshes() const noexcept { return meshes; };
    };

    class Model : public AbstractModel {
    protected:
        std::vector<std::shared_ptr<Material>> materials;
    public:
        explicit Model(decltype(meshes)&& mesh, decltype(materials)&& materials) noexcept
            : materials{std::move(materials)} {
            meshes = std::move(mesh);
        }

        ~Model() override = default;

        Model(const Model&) noexcept = delete;
        Model& operator=(const Model&) noexcept = delete;

        Model(Model&&) noexcept = default;
        Model& operator=(Model&&) noexcept = default;

        [[nodiscard]] const auto& getMaterials() const noexcept { return materials; }
    };

    class Cube : public AbstractModel {
    public:
        Cube() {
            std::vector<Vertex> vertices = {
                // back face
                {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}}, // bottom-left
                {{0.5f, -0.5f, -0.5f},  {1.0f, 0.0f}}, // bottom-right
                {{0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}}, // top-right
                {{0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}}, // top-right
                {{ -0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}}, // top-left
                {{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f}}, // bottom-left
                // front face
                {{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}}, // bottom-left
                {{ 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f}}, // top-right
                {{0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}}, // bottom-right
                {{0.5f,  0.5f,  0.5f},  {1.0f, 1.0f}}, // top-right
                {{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}}, // bottom-left
                {{-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f}}, // top-left
                // left face
                {{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}}, // top-right
                {{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}}, // bottom-left
                {{-0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}}, // top-left
                {{ -0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}}, // bottom-left
                {{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}}, // top-right
                {{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}}, // bottom-right
                // right face
                {{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}}, // top-left
                {{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}}, // top-right
                {{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}}, // bottom-right
                {{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}}, // bottom-right
                {{  0.5f, -0.5f,  0.5f}, { 0.0f, 0.0f}}, // bottom-left
                {{  0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f}}, // top-left
                // bottom face
                {{ -0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}}, // top-right
                {{  0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}}, // bottom-left
                {{  0.5f, -0.5f, -0.5f},  {1.0f, 1.0f}}, // top-left
                {{ 0.5f, -0.5f,  0.5f}, { 1.0f, 0.0f}}, // bottom-left
                {{ -0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f}}, // top-right
                {{ -0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}}, // bottom-right
                // top face
                {{ -0.5f,  0.5f, -0.5f},  {0.0f, 1.0f}}, // top-left
                {{ 0.5f,  0.5f, -0.5f}, { 1.0f, 1.0f}}, // top-right
                {{ 0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f}}, // bottom-right
                {{ 0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f}}, // bottom-right
                {{ -0.5f,  0.5f,  0.5f}, { 0.0f, 0.0f}}, // bottom-left
                {{ -0.5f,  0.5f, -0.5f}, { 0.0f, 1.0f}}  // top-left
            };

            auto mesh = std::make_shared<Mesh<Vertex>>(std::move(vertices), "temp", MeshDataType::Static, DrawMode::Triangles);
            meshes.emplace_back(mesh);
        }
    };
}