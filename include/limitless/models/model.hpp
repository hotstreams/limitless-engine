#pragma once

#include <limitless/models/abstract_model.hpp>

namespace Limitless::ms {
    class Material;
}

namespace Limitless {
    class Model : public AbstractModel {
    protected:
        std::vector<std::shared_ptr<ms::Material>> materials;

        Model(decltype(meshes)&& mesh, decltype(materials)&& materials, std::string name);
    public:
        ~Model() override = default;

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        Model(Model&&) = default;
        Model& operator=(Model&&) = default;

        [[nodiscard]] const auto& getMaterials() const noexcept { return materials; }
        [[nodiscard]] auto& getMaterials() noexcept { return materials; }

        class Builder;
        static Builder builder();
    };
}