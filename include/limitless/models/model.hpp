#pragma once

#include "limitless/models/mesh.hpp"
#include "limitless/util/lod_selection.h"
#include "limitless/util/lod_transition.h"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Limitless::ms {
    class Material;
}

namespace Limitless {
    /**
     * KHR_materials_variants: materials per glTF variant index, keyed by mesh name (LOD0 naming).
     * nullptr entries in the vector mean "use the mesh default material".
     */
    struct ModelMaterialVariantSet {
        std::vector<std::string> variant_names;
        std::unordered_map<std::string, std::vector<std::shared_ptr<ms::Material>>> materials_by_mesh_name;
    };

    /** Paths to offline billboard atlas outputs (last-LOD impostor), optional on Model. */
    struct BillboardLodBundle {
        std::filesystem::path manifest_json;
        std::filesystem::path color_png;
        std::filesystem::path normal_png;
        std::filesystem::path properties_png;
    };

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
        /// Fraction (0,1] of each LOD distance band used as cross-fade zone toward the next coarser LOD.
        float lod_fade_transition_width;
        std::shared_ptr<const ModelMaterialVariantSet> material_variant_set_;
        std::shared_ptr<const BillboardLodBundle> billboard_lod_bundle_;
    protected:
        Model(
            const std::string& name,
            const std::vector<std::shared_ptr<Mesh>>& meshes,
            const std::vector<std::shared_ptr<ms::Material>>& materials,
            LodTransition transition,
            LodSelection selection,
            const std::vector<float>& distances,
            float lod_fade_transition_width,
            std::shared_ptr<const ModelMaterialVariantSet> material_variants = nullptr,
            std::shared_ptr<const BillboardLodBundle> billboard_bundle = nullptr
        );

        Model(
            const std::string& name,
            const std::vector<Lod>& lods,
            LodTransition transition,
            LodSelection selection,
            const std::vector<float>& distances,
            float lod_fade_transition_width,
            std::shared_ptr<const ModelMaterialVariantSet> material_variants = nullptr,
            std::shared_ptr<const BillboardLodBundle> billboard_bundle = nullptr
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
        [[nodiscard]] float getLodFadeTransitionWidth() const noexcept { return lod_fade_transition_width; }

        [[nodiscard]] const ModelMaterialVariantSet* getMaterialVariantSet() const noexcept {
            return material_variant_set_.get();
        }
        [[nodiscard]] const BillboardLodBundle* getBillboardLodBundle() const noexcept {
            return billboard_lod_bundle_.get();
        }

        class Builder;
        static Builder builder();
    };
}