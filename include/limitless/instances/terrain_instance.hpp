#pragma once

#include <limitless/instances/model_instance.hpp>
#include <utility>
#include <limitless/models/model.hpp>
#include "../../../src/limitless/util/geoclipmap.h"

#include <limitless/assets.hpp>
#include <limitless/ms/material.hpp>
#include <iostream>

namespace Limitless {
    class TerrainInstance : public ModelInstance {
    public:
        struct data {
            std::shared_ptr<ModelInstance> cross;
            std::vector<std::shared_ptr<ModelInstance>> tiles;
            std::vector<std::shared_ptr<ModelInstance>> fillers;
            std::vector<std::shared_ptr<ModelInstance>> trims;
            std::vector<std::shared_ptr<ModelInstance>> seams;
        } _data;

//        std::shared_ptr<AbstractMesh> tile_mesh;  0
//        std::shared_ptr<AbstractMesh> filler_mesh;    1
//        std::shared_ptr<AbstractMesh> trim_mesh;  2
//        std::shared_ptr<AbstractMesh> cross_mesh; 3
//        std::shared_ptr<AbstractMesh> seam_mesh;  4
//        std::shared_ptr<AbstractMesh> skirt_mesh; 5

        TerrainInstance(decltype(model) model, const glm::vec3& position, data&& instances)
            : ModelInstance {InstanceType::Terrain, std::move(model), position}
            , _data {std::move(instances)} {
        }

        static constexpr int _mesh_size = 64;
        static constexpr int _mesh_lods = 7;
        static constexpr float _mesh_vertex_spacing = 1.0f;

        static std::shared_ptr<TerrainInstance> create(Assets& assets) {
            std::vector<std::shared_ptr<AbstractMesh>> meshes = GeoClipMap::generate(_mesh_size, _mesh_lods);

            std::vector<std::shared_ptr<ms::Material>> materials;
            for (const auto &item: meshes) {
                materials.emplace_back(assets.materials.at("red"));
            }

            data data;

//            _data.cross = RS->instance_create2(_meshes[GeoClipMap::CROSS], scenario);
//            RS->instance_geometry_set_cast_shadows_setting(_data.cross, RenderingServer::ShadowCastingSetting(_cast_shadows));
//            RS->instance_set_layer_mask(_data.cross, _render_layers);

            auto tile_model = std::make_shared<Model>(std::vector{meshes[0]}, std::vector{materials[0]}, "map");
            auto filler_model = std::make_shared<Model>(std::vector{meshes[1]}, std::vector{materials[1]}, "map");
            auto trim_model = std::make_shared<Model>(std::vector{meshes[2]}, std::vector{materials[2]}, "map");
            auto cross_model = std::make_shared<Model>(std::vector{meshes[3]}, std::vector{materials[3]}, "map");
            auto seam_model = std::make_shared<Model>(std::vector{meshes[4]}, std::vector{materials[4]}, "map");

            data.cross = std::make_shared<ModelInstance>(std::move(cross_model), glm::vec3(0.0f));

            for (int l = 0; l < _mesh_lods; l++) {
                for (int x = 0; x < 4; x++) {
                    for (int y = 0; y < 4; y++) {
                        if (l != 0 && (x == 1 || x == 2) && (y == 1 || y == 2)) {
                            continue;
                        }

//                        RID tile = RS->instance_create2(_meshes[GeoClipMap::TILE], scenario);
//                        RS->instance_geometry_set_cast_shadows_setting(tile, RenderingServer::ShadowCastingSetting(_cast_shadows));
//                        RS->instance_set_layer_mask(tile, _render_layers);
//                        _data.tiles.push_back(tile);
                        data.tiles.emplace_back(std::make_shared<ModelInstance>(tile_model, glm::vec3(0.0f)));
                    }
                }

//                RID filler = RS->instance_create2(_meshes[GeoClipMap::FILLER], scenario);
//                RS->instance_geometry_set_cast_shadows_setting(filler, RenderingServer::ShadowCastingSetting(_cast_shadows));
//                RS->instance_set_layer_mask(filler, _render_layers);
//                _data.fillers.push_back(filler);

                data.fillers.emplace_back(std::make_shared<ModelInstance>(filler_model, glm::vec3(0.0f)));

                if (l != _mesh_lods - 1) {
//                    RID trim = RS->instance_create2(_meshes[GeoClipMap::TRIM], scenario);
//                    RS->instance_geometry_set_cast_shadows_setting(trim, RenderingServer::ShadowCastingSetting(_cast_shadows));
//                    RS->instance_set_layer_mask(trim, _render_layers);
//                    _data.trims.push_back(trim);
                    data.trims.emplace_back(std::make_shared<ModelInstance>(trim_model, glm::vec3(0.0f)));

//                    RID seam = RS->instance_create2(_meshes[GeoClipMap::SEAM], scenario);
//                    RS->instance_geometry_set_cast_shadows_setting(seam, RenderingServer::ShadowCastingSetting(_cast_shadows));
//                    RS->instance_set_layer_mask(seam, _render_layers);
//                    _data.seams.push_back(seam);
                    data.seams.emplace_back(std::make_shared<ModelInstance>(seam_model, glm::vec3(0.0f)));
                }
            }

//            std::vector<ModelInstance> instances;
//            for (int i = 0; i < meshes.size(); ++i) {
//                auto model = std::make_shared<Model>(std::vector{meshes[i]}, std::vector{materials[i]}, "map");
//                instances.emplace_back(model, glm::vec3{0.0f});
//            }

            auto model = std::make_shared<Model>(std::move(meshes), std::move(materials), "map");
            return std::make_shared<TerrainInstance>(std::move(model), glm::vec3(0.0f), std::move(data));
        }

        void update(const Limitless::Camera &camera) override;

        void snap(const Camera& p_cam_pos) {
            std::cout << "snap" << std::endl;
            glm::vec3 cam_pos = p_cam_pos.getPosition();
            cam_pos.y = 0;

//            tile_mesh,    0
//            filler_mesh,  1
//            trim_mesh,    2
//            cross_mesh,   3
//            seam_mesh,    4

            glm::vec3 snapped_pos = glm::floor(cam_pos / _mesh_vertex_spacing) * _mesh_vertex_spacing;
//            Transform3D t = Transform3D().scaled(glm::vec3(_mesh_vertex_spacing, 1, _mesh_vertex_spacing));
//            t.origin = snapped_pos;
//            RS->instance_set_transform(_data.cross, t);

            _data.cross->setPosition(snapped_pos);
            _data.cross->setScale(glm::vec3(_mesh_vertex_spacing, 1, _mesh_vertex_spacing));

            int edge = 0;
            int tile = 0;

            for (int l = 0; l < _mesh_lods; l++) {
                float scale = float(1 << l) * _mesh_vertex_spacing;
                snapped_pos = glm::floor(cam_pos / scale) * scale;
                glm::vec3 tile_size = glm::vec3(float(_mesh_size << l), 0, float(_mesh_size << l)) * _mesh_vertex_spacing;
                glm::vec3 base = snapped_pos - glm::vec3(float(_mesh_size << (l + 1)), 0.f, float(_mesh_size << (l + 1))) * _mesh_vertex_spacing;

                // Position tiles
                for (int x = 0; x < 4; x++) {
                    for (int y = 0; y < 4; y++) {
                        if (l != 0 && (x == 1 || x == 2) && (y == 1 || y == 2)) {
                            continue;
                        }

                        glm::vec3 fill = glm::vec3(x >= 2 ? 1.f : 0.f, 0.f, y >= 2 ? 1.f : 0.f) * scale;
                        glm::vec3 tile_tl = base + glm::vec3(x, 0.f, y) * tile_size + fill;
                        glm::vec3 tile_br = tile_tl + tile_size;

//                        Transform3D t = Transform3D().scaled(glm::vec3(scale, 1.f, scale));
//                        t.origin = tile_tl;
//
//                        RS->instance_set_transform(_data.tiles[tile], t);

                        _data.tiles[tile]->setPosition(tile_tl);
                        _data.tiles[tile]->setScale(glm::vec3(scale, 1.f, scale));

                        tile++;
                    }
                }

                {
//                    Transform3D t = Transform3D().scaled(glm::vec3(scale, 1.f, scale));
//                    t.origin = snapped_pos;
//                    RS->instance_set_transform(_data.fillers[l], t);

                    _data.fillers[l]->setPosition(snapped_pos);
                    _data.fillers[l]->setScale(glm::vec3(scale, 1.f, scale));
                }

                if (l != _mesh_lods - 1) {
                    float next_scale = scale * 2.0f;
                    glm::vec3 next_snapped_pos = glm::floor(cam_pos / next_scale) * next_scale;

                    // Position trims
                    {
                        glm::vec3 tile_center = snapped_pos + (glm::vec3(scale, 0.f, scale) * 0.5f);
                        glm::vec3 d = cam_pos - next_snapped_pos;

                        int r = 0;
                        r |= d.x >= scale ? 0 : 2;
                        r |= d.z >= scale ? 0 : 1;

                        float rotations[4] = { 0.f, 270.f, 90.f, 180.f };

                        float angle = glm::radians(rotations[r]);
//                        Transform3D t = Transform3D().rotated(glm::vec3(0.f, 1.f, 0.f), -angle);
//                        t = t.scaled(glm::vec3(scale, 1.f, scale));
//                        t.origin = tile_center;
//                        RS->instance_set_transform(_data.trims[edge], t);

                        _data.trims[edge]->setPosition(tile_center);
                        _data.trims[edge]->setScale(glm::vec3(scale, 1.f, scale));
                        _data.trims[edge]->setRotation(glm::vec3(0.f, -angle, 0.f));
                    }

                    // Position seams
                    {
                        glm::vec3 next_base = next_snapped_pos - glm::vec3(float(_mesh_size << (l + 1)), 0.f, float(_mesh_size << (l + 1))) * _mesh_vertex_spacing;
//                        Transform3D t = Transform3D().scaled(glm::vec3(scale, 1.f, scale));
//                        t.origin = next_base;
//                        RS->instance_set_transform(_data.seams[edge], t);

                        _data.seams[edge]->setPosition(next_base);
                        _data.seams[edge]->setScale(glm::vec3(scale, 1.f, scale));
                    }
                    edge++;
                }
            }
        }
    };
}