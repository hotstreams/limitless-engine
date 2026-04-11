#include <limitless/instances/lod_group_instance.hpp>
#include <limitless/models/model.hpp>
#include <algorithm>
#include <cmath>
#include <stdexcept>

using namespace Limitless;

namespace {
    // Simple hysteresis to avoid LOD flicker around thresholds (outside cross-fade bands).
    constexpr float LOD_HYSTERESIS_RATIO = 0.10f;
}

LodGroupInstance::LodGroupInstance(const std::shared_ptr<Model>& model)
    : current_lod {0}
    , lod_selection {model->getSelection()}
    , lod_transition {model->getTransition()}
    , distances {model->getDistances()}
    , fade_transition_width {model->getLodFadeTransitionWidth()} {
    for (const auto& [meshes, materials] : model->getLods()) {
        Lod lod;

        for (uint32_t i = 0; i < meshes.size(); ++i) {
            const auto& mesh = meshes.at(i);
            const auto& material = materials.at(i);
            lod.emplace(mesh->getName(), MeshInstance {mesh, std::make_shared<ms::Material>(*material)});
        }

        lods.emplace_back(std::move(lod));
    }
}

void LodGroupInstance::computeCrossFade(float distance) {
    crossfade_active = false;
    if (!crossfade_effect_enabled || lod_transition != LodTransition::CrossFadeDither || lods.size() < 2 || distances.empty()) {
        return;
    }

    const float w = glm::clamp(fade_transition_width, 0.001f, 1.0f);

    for (uint32_t i = 0; i + 1 < static_cast<uint32_t>(lods.size()) && i < distances.size(); ++i) {
        const float T = distances[i];
        const float prevT = (i == 0) ? 0.0f : distances[i - 1];

        float nextT;
        if (i + 1 < distances.size()) {
            nextT = distances[i + 1];
        } else {
            nextT = T + std::max(1.0f, T - prevT) * 10.0f;
        }

        const float segLeft = std::max(T - prevT, 0.001f);
        const float segRight = std::max(nextT - T, 0.001f);
        const float blendOut = std::max(0.001f, w * segLeft);
        const float blendIn = std::max(0.001f, w * segRight);

        if (distance > T - blendOut && distance <= T) {
            crossfade_active = true;
            crossfade_finer_lod = i;
            crossfade_coarser_lod = i + 1;
            crossfade_t = (distance - (T - blendOut)) / blendOut;
            return;
        }
        if (distance > T && distance <= T + blendIn) {
            crossfade_active = true;
            crossfade_finer_lod = i;
            crossfade_coarser_lod = i + 1;
            crossfade_t = 1.0f - (distance - T) / blendIn;
            return;
        }
    }
}

glm::vec4 LodGroupInstance::getLodFadePackedForDrawLod(uint32_t draw_lod_index) const noexcept {
    if (!crossfade_active) {
        return glm::vec4(0.0f);
    }
    if (draw_lod_index == crossfade_finer_lod) {
        return glm::vec4(crossfade_t, 0.0f, 0.0f, 1.0f);
    }
    if (draw_lod_index == crossfade_coarser_lod) {
        return glm::vec4(crossfade_t, 1.0f, 0.0f, 1.0f);
    }
    return glm::vec4(0.0f);
}

const std::unordered_map<std::string, MeshInstance>& LodGroupInstance::getLodMeshesAt(uint32_t lod_index) const {
    return lods.at(lod_index);
}

void LodGroupInstance::applyMaterialVariantFromModel(const Model& model, size_t variant_index) {
    const auto* vs = model.getMaterialVariantSet();
    if (!vs) {
        throw std::runtime_error("LodGroupInstance::applyMaterialVariantFromModel: model has no material variant set");
    }
    if (variant_index >= vs->variant_names.size()) {
        throw std::out_of_range("material variant index");
    }
    for (auto& lod : lods) {
        for (auto& [mesh_name, mesh_inst] : lod) {
            auto it = vs->materials_by_mesh_name.find(mesh_name);
            if (it == vs->materials_by_mesh_name.end()) {
                continue;
            }
            const auto& row = it->second;
            if (variant_index >= row.size()) {
                continue;
            }
            const auto& m = row[variant_index];
            if (m) {
                mesh_inst.changeMaterial(m);
            } else {
                mesh_inst.reset();
            }
        }
    }
}

void LodGroupInstance::selectLod(const Camera& camera, const glm::vec3& position) {
    switch (lod_selection) {
        case LodSelection::CameraDistance:
            {
                if (lods.size() <= 1 || distances.empty()) {
                    current_lod = 0;
                    break;
                }

                const float distance = glm::distance(camera.getPosition(), position);

                uint32_t desired = static_cast<uint32_t>(lods.size() - 1);
                const auto limit = static_cast<uint32_t>(std::min(distances.size(), lods.size()));
                for (uint32_t i = 0; i < limit; ++i) {
                    if (distance <= distances[i]) {
                        desired = i;
                        break;
                    }
                }

                if (desired == current_lod) {
                    break;
                }

                const uint32_t boundary = std::min(current_lod, desired);
                if (boundary >= distances.size()) {
                    current_lod = desired;
                    break;
                }

                const float threshold = distances[boundary];
                if (threshold <= 0.0f) {
                    current_lod = desired;
                    break;
                }

                if (desired > current_lod) {
                    if (distance > threshold * (1.0f + LOD_HYSTERESIS_RATIO)) {
                        current_lod = desired;
                    }
                } else {
                    if (distance < threshold * (1.0f - LOD_HYSTERESIS_RATIO)) {
                        current_lod = desired;
                    }
                }
            }
            break;
    }
}

void LodGroupInstance::update(const Camera& camera, const glm::vec3& position) {
    const float distance = glm::distance(camera.getPosition(), position);
    computeCrossFade(distance);
    selectLod(camera, position);

    for (auto& [_, mesh] : lods[current_lod]) {
        mesh.update();
    }

    if (crossfade_active) {
        if (crossfade_finer_lod != current_lod) {
            for (auto& [_, mesh] : lods[crossfade_finer_lod]) {
                mesh.update();
            }
        }
        if (crossfade_coarser_lod != current_lod && crossfade_coarser_lod != crossfade_finer_lod) {
            for (auto& [_, mesh] : lods[crossfade_coarser_lod]) {
                mesh.update();
            }
        }
    }
}
