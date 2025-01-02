#include "scene.hpp"

#include <limitless/scene.hpp>
#include <limitless/instances/instanced_instance.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/lighting/light.hpp>
#include <random>
#include <limitless/core/vertex.hpp>
#include <limitless/instances/terrain_instance.hpp>
#include <limitless/util/noise.hpp>

using namespace LimitlessMaterials;
using namespace Limitless;

LimitlessMaterials::Scene::Scene(Limitless::Context& ctx, Limitless::Assets& assets)
    : scene(ctx)
    , assets {assets} {

    scene.add(Instance::builder()
        .model(assets.models.at("cube"))
        .position({64.0f, 10.0f, 0.0f})
        .scale(glm::vec3{128.0f})
        .material(assets.materials.at("test"))
        .build()
    );

    scene.getLighting().setAmbientColor(glm::vec4(1.0f));

    scene.add(Light::builder()
      .color(glm::vec4(1.0, 1.0, 1.0, 1.0f))
      .direction(glm::vec3{0.3f, -1.0f, 0.0f})
      .build()
    );

    std::mt19937 rng;

    float chunk_size = 1024.0f;

    auto elevation_map = Noise::makePerlinNoiseMap(glm::vec2{chunk_size}, rng, 8);
    auto humidity_map = Noise::makePerlinNoiseMap(glm::vec2{chunk_size}, rng, 8);
    auto test_map = Noise::makePerlinNoiseMap(glm::vec2{chunk_size}, rng, 8);

    auto* elevations = new float[chunk_size * chunk_size];
    auto* controls = new Limitless::TerrainInstance::control_value[chunk_size * chunk_size];

    for (auto y = 0; y < chunk_size; ++y) {
        for (auto x = 0; x < chunk_size; ++x) {
            const auto elevation = elevation_map[y][x];
            const auto humidity = humidity_map[y][x];
            const auto test = test_map[y][x];

            *(elevations + int(y * chunk_size) + x) = elevation;

            Limitless::TerrainInstance::control_value v {0u, 0u, 0u, 0u};

            if (elevation >= 0.75f) {
                v.base_id = 2;
            } else if (humidity >= 0.2f) {
                v.base_id = 1;
            } else {
                v.base_id = 0;
            }

            if (test >= 0.5 && test <= 0.7) {
                v.extra_id = 2;
                v.blend = 128;
            }

            *(controls + int(y * chunk_size) + x) = v;
        }
    }

    scene.add(
        Instance::builder()
            .chunk_size(chunk_size)
            .vertex_spacing(1.0f)
            .height(elevations)
            .height_scale(20.0f)
            .mesh_size(32.0f)
            .mesh_lod_count(5)
            .control(controls)
            .noise(TextureLoader::load(assets, "../../assets/textures/noise.png"))
            .albedo_map(Limitless::TextureLoader::load(
                assets, {
                    "../../assets/textures/dirt.png",
                    "../../assets/textures/grass.png",
                    "../../assets/textures/rock.png"
                },
                Limitless::TextureLoaderFlags(Limitless::TextureLoaderFlags::Space::sRGB)
            ))
            .normal_map(Limitless::TextureLoader::load(
                assets, {
                    "../../assets/textures/dirt_normal.png",
                    "../../assets/textures/grass_normal.png",
                    "../../assets/textures/rock_normal.png"
                }
            ))
            .orm_map(Limitless::TextureLoader::load(
                assets, {
                    "../../assets/textures/dirt_orm.png",
                    "../../assets/textures/grass_orm.png",
                    "../../assets/textures/rock_orm.png"
                }
            ))
            .asTerrain(assets)
    );

    delete[] elevations;
    delete[] controls;
}

void LimitlessMaterials::Scene::update(Limitless::Context& context, const Limitless::Camera& camera) {

}