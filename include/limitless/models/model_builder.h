#pragma once

#include <limitless/assets.hpp>
#include <limitless/models/model.hpp>

#include "skeletal_model.hpp"

namespace Limitless {
    class Model::Builder {
    private:
        std::string name_;
        std::vector<std::shared_ptr<Mesh>> meshes_;
        std::vector<std::shared_ptr<ms::Material>> materials_;
        LodTransition transition_ {LodTransition::None};
        LodSelection selection_ {LodSelection::CameraDistance};
        float lod_fade_transition_width_ {0.25f};
        std::unordered_map<std::string, uint32_t> bone_map_;
        std::vector<Animation> animations_;
        std::vector<Bone> bones_;
        std::vector<Tree<uint32_t>> skeletons_;
        bool batched_ {true};

        std::vector<std::shared_ptr<Model>> models_;
        std::vector<float> lod_distances_;
        std::vector<std::shared_ptr<ms::Material>> lod_material_overrides_;
    public:
        Builder& name(const std::string& name);
        Builder& meshes(const std::vector<std::shared_ptr<Mesh>>& meshes);
        Builder& materials(const std::vector<std::shared_ptr<ms::Material>>& materials);
        Builder& bone_map(std::unordered_map<std::string, uint32_t>&& bone_map);
        Builder& animations(std::vector<Animation>&& animations);
        Builder& bones(std::vector<Bone>&& bones);
        Builder& skeletons(std::vector<Tree<uint32_t>>&& skeletons);
        Builder& transition(LodTransition transition);
        Builder& selection(LodSelection selection);
        /// Portion of each LOD band (between distance thresholds) used as cross-fade zone; only for CrossFadeDither.
        Builder& lod_fade_transition_width(float width);
        Builder& batched();

        /**
         * Adds a model as a LOD level with an upper-bound distance.
         *
         * Distance convention (CameraDistance):
         * - LOD i is selected when distance_to_camera <= distance[i]
         * - If distance_to_camera is greater than all distances, the last LOD is selected
         */
        Builder& add_lod(const std::shared_ptr<Model>& model, float distance);

        /**
         * Adds a model as a LOD level with an upper-bound distance, forcing all meshes to use the specified material.
         */
        Builder& add_lod(const std::shared_ptr<Model>& model, const std::shared_ptr<ms::Material>& material, float distance);

        /**
         * Adds multiple LOD levels at once with matching distances.
         */
        Builder& add_lods(const std::vector<std::shared_ptr<Model>>& models, const std::vector<float>& distances);

        std::shared_ptr<Model> build(Assets& assets);
    };
}

