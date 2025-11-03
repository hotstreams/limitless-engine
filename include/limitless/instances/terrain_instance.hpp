#pragma once

#include <limitless/instances/model_instance.hpp>
#include <utility>
#include <limitless/models/model.hpp>

#include <limitless/assets.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/core/texture/texture_builder.hpp>

#include <limitless/util/brush.hpp>
#include "instanced_instance.hpp"

namespace Limitless {
    class Assets;

    class TerrainInstance : public Instance {
    public:
        static constexpr uint8_t MAX_TEXTURES = 64;

        struct control_value final {
            uint32_t base_id    : 6;   // 0-5
            uint32_t extra_id   : 6;   // 6-11
            uint32_t blend      : 8;   // 12-19
            uint32_t scale      : 3;   // 20-22
            uint32_t rotation   : 4;   // 23-26
            uint32_t reserved   : 5;   // 27-31

            static uint32_t encode(control_value ctrl) {
                uint32_t result = 0;
                result |= (ctrl.base_id & 0x3F);
                result |= (ctrl.extra_id & 0x3F) << 6;
                result |= (ctrl.blend & 0xFF) << 12;
                result |= (ctrl.scale & 0x7) << 20;
                result |= (ctrl.rotation & 0xF) << 23;
                result |= (ctrl.reserved & 0x1F) << 27;
                return result;
            }

            static control_value decode(uint32_t value) {
                control_value ctrl {};
                ctrl.base_id = value & 0x3F;
                ctrl.extra_id = (value >> 6) & 0x3F;
                ctrl.blend = (value >> 12) & 0xFF;
                ctrl.scale = (value >> 20) & 0x7;
                ctrl.rotation = (value >> 23) & 0xF;
                ctrl.reserved = (value >> 27) & 0x1F;
                return ctrl;
            }
        };
    private:
        // terrain size in number of vertices
        float terrain_size = 128.0f;

        // spacing between vertices
        float vertex_spacing = 1.0f;

        // height scale factor
        float height_scale = 1.0f;

        // climap mesh size
        int mesh_size = 32;

        // clipmap lod count
        int mesh_lod_count = 6;

        // distance at which mipmap starts
        float bias_distance = 64.0f;

        float mipmap_bias = 1.0f;

        float depth_blur = 0.0f;

        float blend_sharpness = 4.0f;

        uint32_t enable_tile_bilerp = 1;

        float normal_bilerp_multiplier = 1.0f;

        float tile_bilerp_multiplier = 1.0f;

        struct Mesh {
            std::shared_ptr<ModelInstance> cross;
            std::shared_ptr<InstancedInstance> tiles;
            std::shared_ptr<InstancedInstance> fillers;
            std::shared_ptr<InstancedInstance> trims;
            std::shared_ptr<InstancedInstance> seams;
        } mesh;

        std::shared_ptr<Texture> height_map;
        std::shared_ptr<Texture> control;
        std::shared_ptr<Texture> albedo;
        std::shared_ptr<Texture> normal;
        std::shared_ptr<Texture> color_map;

        std::vector<float> texture_uv_scale;
        std::vector<float> texture_normal_depth;
        std::vector<glm::vec2> texture_detile;

        void snap(const Camera& p_cam_pos);
    public:
        TerrainInstance(
            std::shared_ptr<Texture> height_map,
            std::shared_ptr<Texture> control_map,
            std::shared_ptr<Texture> albedo_map,
            std::shared_ptr<Texture> normal_map,
            std::shared_ptr<Texture> color_map
        );

        void initializeMesh(Assets& assets);

        void setHeightMap(const std::shared_ptr<Texture>& height_map);
        
        void setTextureUVScales(const std::vector<float>& scales);
        void setTextureNormalDepths(const std::vector<float>& normal_depths);
        void setTextureDetiles(const std::vector<glm::vec2>& normal_depths);


        void setTextureColors(const std::vector<glm::vec4>& colors);

        void setControlMap(const std::shared_ptr<Texture>& control_map);
        void setAlbedoMap(const std::shared_ptr<Texture>& albedo_map);
        void setNormalMap(const std::shared_ptr<Texture>& normal_map);
        void setColorMap(const std::shared_ptr<Texture>& color_map);

        void setVertexSpacing(float vertex_spacing);

        void setMeshSize(int mesh_size);
        void setMeshLodCount(int mesh_lod_Count);
        void setHeightScale(float height);
        void setTerrainSize(float size);

        void enableTileBilerp();
        void disableTileBilerp();
        void setTileBilerp(bool bilerp);

        void setNormalBilerpMultiplier(float multiplier);
        void setTileBilerpMultiplier(float multiplier);

        /**
         * Updates full height map from data
         *
         * expects pointer to UNSIGNED CHAR / std::byte
         *
         * you should convert your normalized floats in range [0, 1] to unsigned bytes [0, 255]
         *
         * to get height more than 1.0 set height_scale
         */
        void updateHeight(const void* data) const;

        /**
         * Updates part of height map with offset and size
         */
        void updateHeight(glm::uvec2 offset, glm::uvec2 size, const void* data) const;

        /**
         * Updates full control map from data
         *
         * expects pointer to TerrainInstance::control_value / uint32_t
         */
        void updateControl(const void* data) const;

        /**
         * Updates part of control map with offset and size
         */
        void updateControl(glm::uvec2 offset, glm::uvec2 size, const void* data) const;

        [[nodiscard]] const auto& getMesh() const noexcept { return mesh; }

        void update(const Camera &camera) override;

        std::unique_ptr<Instance> clone() noexcept override {
            return std::make_unique<TerrainInstance>(*this);
        }
    };
}