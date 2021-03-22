#pragma once

#include <limitless/models/mesh.hpp>
#include <limitless/material_system/material.hpp>

namespace LimitlessEngine {
    class AbstractModel {
    protected:
        std::vector<std::shared_ptr<AbstractMesh>> meshes {};
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
        [[nodiscard]] auto& getMaterials() noexcept { return materials; }
    };
}