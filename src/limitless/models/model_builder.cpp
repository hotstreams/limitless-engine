#include <stdexcept>
#include <limitless/models/model_builder.h>
#include <limitless/models/mesh_builder.hpp>
#include <limitless/ms/material_builder.hpp>

#include "../../../samples/effects/assets.hpp"

using namespace Limitless;

Model::Builder& Model::Builder::name(const std::string &name) {
    name_ = name;
    return *this;
}

Model::Builder& Model::Builder::materials(const std::vector<std::shared_ptr<ms::Material> > &materials) {
    materials_ = materials;
    return *this;
}

Model::Builder & Model::Builder::bone_map(std::unordered_map<std::string, uint32_t>&& bone_map) {
    bone_map_ = std::move(bone_map);
    return *this;
}

Model::Builder & Model::Builder::animations(std::vector<Animation>&& animations) {
    animations_ = std::move(animations);
    return *this;
}

Model::Builder & Model::Builder::bones(std::vector<Bone>&& bones) {
    bones_ = std::move(bones);
    return *this;
}

Model::Builder & Model::Builder::skeletons(std::vector<Tree<uint32_t>>&& skeletons) {
    skeletons_ = std::move(skeletons);
    return *this;
}

Model::Builder& Model::Builder::meshes(const std::vector<std::shared_ptr<Mesh>> &meshes) {
    meshes_ = meshes;
    return *this;
}

Model::Builder &Model::Builder::batched() {
    batched_ = true;
    return *this;
}

Model::Builder& Model::Builder::transition(LodTransition transition) {
    transition_ = transition;
    return *this;
}

Model::Builder& Model::Builder::selection(LodSelection selection) {
    selection_ = selection;
    return *this;
}

Model::Builder& Model::Builder::distances(const std::vector<float>& distances) {
    distances_ = distances;
    return *this;
}

Model::Builder& Model::Builder::add_lod(const std::shared_ptr<Model>& model) {
    models_.emplace_back(model);
    return *this;
}

Model::Builder& Model::Builder::add_lods(const std::vector<std::shared_ptr<Model>>& models) {
    models_.insert(models_.end(), models.begin(), models.end());
    return *this;
}

std::shared_ptr<Model> Model::Builder::build([[maybe_unused]] Assets& assets) {
    // if (name_.empty()) {
    //     throw std::runtime_error("Model name cannot be empty.");
    // }
    //
    // if (meshes_.empty()) {
    //     throw std::runtime_error("Model meshes cannot be empty.");
    // }
    //
    // if (materials_.empty()) {
    //     throw std::runtime_error("Model materials cannot be empty.");
    // }
    //
    // if (meshes_.size() != materials_.size()) {
    //     throw std::runtime_error("Model meshes must have the same number of materials.");
    // }

    // if (batched_) {
    //     auto mesh_builder = Mesh::builder().name(name_ + "_batched");
    //     for (auto& mesh : meshes_) {
    //         mesh_builder.batch(mesh);
    //     }
    //     auto batched_mesh = mesh_builder.build();
    //
    //     auto material_builder = ms::Material::builder();
    //     for (auto material : materials_) {
    //         material_builder.add_batch(material);
    //     }
    //     auto batched_material = material_builder.buildBatched(assets);
    //
    //     if (!skeletons_.empty()) {
    //         return std::shared_ptr<SkeletalModel>(new SkeletalModel(
    //             {batched_mesh},
    //             {batched_material},
    //             std::move(bones_),
    //             std::move(bone_map_),
    //             std::move(skeletons_),
    //             std::move(animations_),
    //             std::move(name_)
    //         ));
    //     }
    //
    //     return std::shared_ptr<Model>(new Model({batched_mesh}, {batched_material}, std::move(name_)));
    // } else {

    if (models_.empty()) {
        if (!skeletons_.empty()) {
            return std::shared_ptr<SkeletalModel>(new SkeletalModel(
                name_,
                std::move(meshes_),
                std::move(materials_),
                transition_,
                selection_,
                std::move(distances_),
                std::move(bones_),
                std::move(bone_map_),
                std::move(skeletons_),
                std::move(animations_)
                ));
        } else {
            return std::shared_ptr<Model>(
                new Model(
                    name_,
                    std::move(meshes_),
                    std::move(materials_),
                    transition_,
                    selection_,
                    std::move(distances_)
                ));
        }
    } else {

        std::vector<Lod> lods;
        for (auto& model : models_) {
            auto& meshes = model->getLods().at(0).meshes;
            auto& materials = model->getLods().at(0).materials;
            lods.emplace_back(Lod{meshes, materials});
        }

        return std::shared_ptr<Model>(new Model(name_, lods, transition_, selection_, distances_));
    }
}
