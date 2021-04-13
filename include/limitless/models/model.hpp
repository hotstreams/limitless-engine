#pragma once

#include <limitless/models/abstract_model.hpp>

namespace LimitlessEngine {
    class Material;

    class Model : public AbstractModel {
    protected:
        std::vector<std::shared_ptr<Material>> materials;
    public:
        Model(decltype(meshes)&& mesh, decltype(materials)&& materials);
        ~Model() override = default;

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        Model(Model&&) = default;
        Model& operator=(Model&&) = default;

        [[nodiscard]] const auto& getMaterials() const noexcept { return materials; }
        [[nodiscard]] auto& getMaterials() noexcept { return materials; }
    };
}