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
        LodTransition transition_;
        LodSelection selection_;
        std::vector<float> distances_;
        std::unordered_map<std::string, uint32_t> bone_map_;
        std::vector<Animation> animations_;
        std::vector<Bone> bones_;
        std::vector<Tree<uint32_t>> skeletons_;
        bool batched_ {true};

        std::vector<std::shared_ptr<Model>> models_;
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
        Builder& distances(const std::vector<float>& distances);
        Builder& batched();

        Builder& add_lod(const std::shared_ptr<Model>& model);
        Builder& add_lods(const std::vector<std::shared_ptr<Model>>& models);

        std::shared_ptr<Model> build(Assets& assets);
    };
}

