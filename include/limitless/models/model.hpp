#pragma once

#include "limitless/models/mesh.hpp"
#include "limitless/util/lod_selection.h"
#include "limitless/util/lod_transition.h"

namespace Limitless::ms {
    class Material;
}

namespace Limitless {
    class Model {
    public:
        struct Lod
        {
            std::vector<std::shared_ptr<Mesh>> meshes;
            std::vector<std::shared_ptr<ms::Material>> materials;
        };
    private:
        std::string name;
        Box bounding_box;
        std::vector<Lod> lods;
        LodTransition transition;
        LodSelection selection;
        std::vector<float> distances;
    protected:
        Model(
            const std::string& name,
            const std::vector<std::shared_ptr<Mesh>>& meshes,
            const std::vector<std::shared_ptr<ms::Material>>& materials,
            LodTransition transition,
            LodSelection selection,
            const std::vector<float>& distances
        );

        Model(
            const std::string& name,
            const std::vector<Lod>& lods,
            LodTransition transition,
            LodSelection selection,
            const std::vector<float>& distances
        );

        void calculateBoundingBox();
        
    public:
        virtual ~Model() = default;

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        Model(Model&&) = default;
        Model& operator=(Model&&) = default;

        [[nodiscard]] const auto& getLods() const noexcept { return lods; }
        [[nodiscard]] auto& getLods() noexcept { return lods; }
        [[nodiscard]] const auto& getName() const noexcept { return name; }
        [[nodiscard]] const auto& getBoundingBox() const noexcept { return bounding_box; }
        [[nodiscard]] LodTransition getTransition() const noexcept { return transition; }
        [[nodiscard]] LodSelection getSelection() const noexcept { return selection; }
        [[nodiscard]] const auto& getDistances() const noexcept { return distances; }

        class Builder;
        static Builder builder();
    };
}