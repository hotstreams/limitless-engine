#pragma once

#include <limitless/instances/model_instance.hpp>
#include <utility>
#include <limitless/models/model.hpp>
#include "../../../src/limitless/util/geoclipmap.h"

#include <limitless/assets.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/core/texture/texture_builder.hpp>

#include <limitless/util/brush.hpp>

namespace Limitless {
    class Assets;

    class TerrainInstance : public Instance {
    public:
        struct control_value final {
            uint32_t base_id    : 6;
            uint32_t extra_id   : 6;
            uint32_t blend      : 8;
            uint32_t reserved   : 12;

            static uint32_t encode(struct control_value ctrl) {
                uint32_t result = 0;
                result |= (ctrl.base_id & 0x3F);
                result |= (ctrl.extra_id & 0x3F) << 6;
                result |= (ctrl.blend & 0xFF) << 12;
                result |= (ctrl.reserved & 0xFFF) << 20;
                return result;
            }

            static control_value decode(uint32_t value) {
                control_value ctrl {};
                ctrl.base_id = value & 0x3F;
                ctrl.extra_id = (value >> 6) & 0x3F;
                ctrl.blend = (value >> 12) & 0xFF;
                ctrl.reserved = (value >> 20) & 0xFFF;
                return ctrl;
            }
        };
    private:
        // size of terrain
        float chunk_size = 1024.0f;

        //
        float chunk_texel_size = 1.0f / chunk_size;

        //
        float vertex_spacing = 0.1f;

        //
        float vertex_density = 1.0f / vertex_spacing;

        //
        float height_scale = 10.0f;

        //
        float noise1_scale = 0.225f;
        float noise2_scale = 0.04f;
        float noise2_angle = 0.0f;
        float noise2_offset = 0.5f;
        float noise3_scale = 0.076f;

        glm::vec3 macro_variation1 = glm::vec3(0.5f);
        glm::vec3 macro_variation2 = glm::vec3(0.33f);

        bool show_tiles = false;

        int mesh_size = 64;

        int mesh_lod_count = 6;

        struct Mesh {
            std::shared_ptr<ModelInstance> cross;
            std::vector<std::shared_ptr<ModelInstance>> tiles;
            std::vector<std::shared_ptr<ModelInstance>> fillers;
            std::vector<std::shared_ptr<ModelInstance>> trims;
            std::vector<std::shared_ptr<ModelInstance>> seams;
        } mesh;

        std::shared_ptr<Texture> height_map;
        std::shared_ptr<Texture> control;
        std::shared_ptr<Texture> albedo;
        std::shared_ptr<Texture> normal;
        std::shared_ptr<Texture> orm;
        std::shared_ptr<Texture> noise;

        void snap(const Camera& p_cam_pos);
    public:
        TerrainInstance(
            std::shared_ptr<Texture> height_map,
            std::shared_ptr<Texture> control_map,
            std::shared_ptr<Texture> albedo_map,
            std::shared_ptr<Texture> normal_map,
            std::shared_ptr<Texture> orm_map,
            std::shared_ptr<Texture> noise
        );

        void initializeMesh(Assets& assets);

        void setHeightMap(const std::shared_ptr<Texture>& height_map);
        void setControlMap(const std::shared_ptr<Texture>& control_map);
        void setNoise(const std::shared_ptr<Texture>& noise);
        void setAlbedoMap(const std::shared_ptr<Texture>& albedo_map);
        void setNormalMap(const std::shared_ptr<Texture>& normal_map);
        void setOrmMap(const std::shared_ptr<Texture>& orm_map);

        void setChunkSize(float chunkSize);

        void setVertexSpacing(float vertexSpacing);

        void setHeightScale(float heightScale);

        void setNoise1Scale(float noise1Scale);

        void setNoise2Scale(float noise2Scale);

        void setNoise2Angle(float noise2Angle);

        void setNoise2Offset(float noise2Offset);

        void setNoise3Scale(float noise3Scale);

        void setMacroVariation1(const glm::vec3 &macroVariation1);

        void setMacroVariation2(const glm::vec3 &macroVariation2);

        void setMeshSize(int meshSize);

        void setMeshLodCount(int meshLodCount);

        /**
         * Updates full height map from data
         *
         * expects pointer to UNSIGNED CHAR / std::byte
         *
         * you should convert your normalized floats in range [0, 1] to unsigned bytes [0, 255]
         *
         * to get height more than 1.0 set height_scale
         */
        void updateHeight(const void* data);

        /**
         * Updates part of height map with offset and size
         */
        void updateHeight(glm::uvec2 offset, glm::uvec2 size, const void* data);

        /**
         * Updates full control map from data
         *
         * expects pointer to TerrainInstance::control_value / uint32_t
         */
        void updateControl(const void* data);

        /**
         * Updates part of control map with offset and size
         */
        void updateControl(glm::uvec2 offset, glm::uvec2 size, const void* data);

        [[nodiscard]] const auto& getMesh() const noexcept { return mesh; }

        void update(const Limitless::Camera &camera) override;

        std::unique_ptr<Instance> clone() noexcept override {
            return std::make_unique<TerrainInstance>(*this);
        }
    };
}