#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <limits>
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

Model::Builder& Model::Builder::lod_fade_transition_width(float width) {
    lod_fade_transition_width_ = width;
    return *this;
}

Model::Builder& Model::Builder::add_lod(const std::shared_ptr<Model>& model, float distance) {
    models_.emplace_back(model);
    lod_distances_.emplace_back(distance);
    lod_material_overrides_.emplace_back(nullptr);
    return *this;
}

Model::Builder& Model::Builder::add_lod(const std::shared_ptr<Model>& model, const std::shared_ptr<ms::Material>& material, float distance) {
    models_.emplace_back(model);
    lod_distances_.emplace_back(distance);
    lod_material_overrides_.emplace_back(material);
    return *this;
}

Model::Builder& Model::Builder::add_lods(const std::vector<std::shared_ptr<Model>>& models, const std::vector<float>& distances) {
    if (models.size() != distances.size()) {
        throw std::runtime_error("Model::Builder::add_lods(): models and distances size mismatch");
    }

    models_.insert(models_.end(), models.begin(), models.end());
    lod_distances_.insert(lod_distances_.end(), distances.begin(), distances.end());
    lod_material_overrides_.insert(lod_material_overrides_.end(), models.size(), nullptr);
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
        // Validation for single-LOD model builds
        if (meshes_.empty()) {
            throw std::runtime_error("Model::Builder::build(): meshes cannot be empty");
        }
        if (materials_.empty()) {
            throw std::runtime_error("Model::Builder::build(): materials cannot be empty");
        }
        if (meshes_.size() != materials_.size()) {
            throw std::runtime_error("Model::Builder::build(): meshes and materials must have the same size");
        }
        for (const auto& mesh : meshes_) {
            if (!mesh) {
                throw std::runtime_error("Model::Builder::build(): mesh cannot be null");
            }
        }
        for (const auto& material : materials_) {
            if (!material) {
                throw std::runtime_error("Model::Builder::build(): material cannot be null");
            }
        }

        if (!skeletons_.empty()) {
            return std::shared_ptr<SkeletalModel>(new SkeletalModel(
                name_,
                std::move(meshes_),
                std::move(materials_),
                transition_,
                selection_,
                {},
                lod_fade_transition_width_,
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
                    {},
                    lod_fade_transition_width_
                ));
        }
    } else {
        // Validation for multi-LOD model builds
        if (models_.size() != lod_distances_.size() || models_.size() != lod_material_overrides_.size()) {
            throw std::runtime_error("Model::Builder::build(): internal LOD arrays size mismatch");
        }
        if (models_.empty()) {
            throw std::runtime_error("Model::Builder::build(): at least one LOD model must be provided");
        }
        if (name_.empty() && models_.front()) {
            name_ = models_.front()->getName();
        }
        if (models_.size() == 1) {
            // not strictly an error, but most users expect 2+ levels for an actual LOD group
        }
        // validate distances are sorted (non-decreasing) and non-negative
        for (size_t i = 0; i < lod_distances_.size(); ++i) {
            const float d = lod_distances_[i];
            if (d < 0.0f || (!std::isfinite(d) && !std::isinf(d))) {
                throw std::runtime_error("Model::Builder::build(): LOD distance must be non-negative (finite or infinity)");
            }
            if (i > 0 && lod_distances_[i] < lod_distances_[i - 1]) {
                throw std::runtime_error("Model::Builder::build(): LOD distances must be sorted in non-decreasing order");
            }
        }

        std::vector<Lod> lods;
        lods.reserve(models_.size());

        for (size_t i = 0; i < models_.size(); ++i) {
            const auto& model = models_[i];
            if (!model) {
                throw std::runtime_error("Model::Builder::build(): LOD model cannot be null");
            }
            if (model->getLods().empty()) {
                throw std::runtime_error("Model::Builder::build(): LOD model must contain at least one lod");
            }

            const auto& meshes = model->getLods().at(0).meshes;
            const auto& materials = model->getLods().at(0).materials;
            if (meshes.empty()) {
                throw std::runtime_error("Model::Builder::build(): LOD model meshes cannot be empty");
            }
            if (materials.size() != meshes.size()) {
                throw std::runtime_error("Model::Builder::build(): LOD model meshes and materials must have the same size");
            }
            for (const auto& mesh : meshes) {
                if (!mesh) {
                    throw std::runtime_error("Model::Builder::build(): LOD model mesh cannot be null");
                }
            }

            const auto& override_material = lod_material_overrides_[i];
            if (override_material) {
                std::vector<std::shared_ptr<ms::Material>> forced(materials.size(), override_material);
                lods.emplace_back(Lod{meshes, std::move(forced)});
            } else {
                for (const auto& material : materials) {
                    if (!material) {
                        throw std::runtime_error("Model::Builder::build(): LOD model material cannot be null (or provide override material)");
                    }
                }
                lods.emplace_back(Lod{meshes, materials});
            }
        }

        return std::shared_ptr<Model>(new Model(name_, lods, transition_, selection_, lod_distances_, lod_fade_transition_width_));
    }
}
