#pragma once

#include <limitless/instances/mesh_instance.hpp>
#include <limitless/camera.hpp>
#include <limitless/util/lod_selection.h>
#include <limitless/util/lod_transition.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>
#include <memory>

namespace Limitless {
    class Model;

    class LodGroupInstance final {
    private:
        using Lod = std::unordered_map<std::string, MeshInstance>;

        std::vector<Lod> lods;
        uint32_t current_lod;

        LodSelection lod_selection;
        LodTransition lod_transition;
        std::vector<float> distances;
        float fade_transition_width;

        bool crossfade_active {false};
        uint32_t crossfade_finer_lod {0};
        uint32_t crossfade_coarser_lod {0};
        /// Weight toward the coarser LOD (0 = all finer, 1 = all coarser) inside the dither cross-fade zone.
        float crossfade_t {0.f};
        bool crossfade_effect_enabled {true};

        void selectLod(const Camera& camera, const glm::vec3& position);
        void computeCrossFade(float distance);

    public:
        explicit LodGroupInstance(const std::shared_ptr<Model>& model);

        void update(const Camera& camera, const glm::vec3& position);

        [[nodiscard]] const auto& getLods() const noexcept { return lods; }
        [[nodiscard]] uint32_t getCurrentLod() const noexcept { return current_lod; }
        [[nodiscard]] const auto& getCurrentMeshes() const noexcept { return lods[current_lod]; }
        [[nodiscard]] auto& getCurrentMeshes() noexcept { return lods[current_lod]; }

        [[nodiscard]] LodTransition getLodTransition() const noexcept { return lod_transition; }
        [[nodiscard]] bool isLodCrossFadeActive() const noexcept { return crossfade_active; }
        [[nodiscard]] uint32_t getCrossFadeFinerLod() const noexcept { return crossfade_finer_lod; }
        [[nodiscard]] uint32_t getCrossFadeCoarserLod() const noexcept { return crossfade_coarser_lod; }

        /// Per-draw SSBO packing: x = crossfade_t, y = 0 finer / 1 coarser pass, w = 1 when cross-fade applies.
        [[nodiscard]] glm::vec4 getLodFadePackedForDrawLod(uint32_t draw_lod_index) const noexcept;

        [[nodiscard]] const std::unordered_map<std::string, MeshInstance>& getLodMeshesAt(uint32_t lod_index) const;

        /// When false, dither cross-fade is skipped (discrete LOD only).
        void setCrossFadeEffectEnabled(bool enabled) noexcept { crossfade_effect_enabled = enabled; }
        [[nodiscard]] bool isCrossFadeEffectEnabled() const noexcept { return crossfade_effect_enabled; }

        /**
         * Applies KHR_materials_variants mapping for one variant index on every stored LOD level.
         * Uses mesh instance names to look up rows in model.getMaterialVariantSet().
         */
        void applyMaterialVariantFromModel(const Model& model, size_t variant_index);
    };
}
