#include "scene.hpp"

#include <limitless/scene.hpp>
#include <limitless/instances/instanced_instance.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/lighting/light.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <random>
#include <limitless/core/vertex.hpp>
#include <limitless/instances/terrain_instance.hpp>
#include <limitless/util/noise.hpp>
#include <limitless/models/plane.hpp>

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

    // Setup skybox for reflections and ambient lighting
    scene.setSkybox(assets.skyboxes.at("skybox"));

    scene.getLighting().setAmbientColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));

    scene.add(Light::builder()
        .color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
        .direction(glm::normalize(glm::vec3{0.5f, -0.7f, 0.3f}))
        .build()
    );

    std::mt19937 rng;

    float chunk_size = 1024.0f;

    auto elevation_map = Noise::makePerlinNoiseMap(glm::vec2{chunk_size}, rng, 8);
    auto humidity_map = Noise::makePerlinNoiseMap(glm::vec2{chunk_size}, rng, 8);
    auto test_map = Noise::makePerlinNoiseMap(glm::vec2{chunk_size}, rng, 8);

    auto* elevations = new float[(int)chunk_size * (int)chunk_size];
    auto* controls = new TerrainInstance::control_value[(int)chunk_size * (int)chunk_size];
    auto* color_map_data = new uint8_t[(int)chunk_size * (int)chunk_size * 4]; // RGBA8 color map

    for (auto y = 0; y < chunk_size; ++y) {
        for (auto x = 0; x < chunk_size; ++x) {
            const auto elevation = elevation_map[y][x];
            const auto humidity = humidity_map[y][x];
            const auto test = test_map[y][x];

            *(elevations + int(y * chunk_size) + x) = elevation;

            TerrainInstance::control_value v {0u, 0u, 0u, 0u, 0u, 0u};

            // Smooth blending between textures for natural transitions
            if (elevation >= 0.8f) {
                // High elevation: Pure rock
                v.base_id = 2;
                v.extra_id = 2;  // Same as base when no blending
                v.blend = 0;
            } else if (elevation >= 0.7f && elevation < 0.8f) {
                // Transition: Dirt to Rock
                v.base_id = 0;     // Dirt base
                v.extra_id = 2;    // Rock overlay
                float blend_factor = (elevation - 0.7f) / 0.1f;
                // Add noise variation for more organic transitions
                blend_factor += (test - 0.5f) * 0.1f; // ±5% variation
                v.blend = static_cast<uint32_t>(glm::clamp(blend_factor, 0.0f, 1.0f) * 255.0f);
            } else if (elevation >= 0.6f && elevation < 0.7f) {
                // Mid elevation: Dirt with grass transitions
                if (humidity >= 0.35f) {
                    // Humid mid-elevation: Pure grass
                    v.base_id = 1;
                    v.extra_id = 1;
                    v.blend = 0;
                } else if (humidity >= 0.25f) {
                    // Transition: Dirt to Grass
                    v.base_id = 0;     // Dirt base
                    v.extra_id = 1;    // Grass overlay
                    float blend_factor = (humidity - 0.25f) / 0.1f;
                    blend_factor += (test - 0.5f) * 0.1f; // ±5% variation
                    v.blend = static_cast<uint32_t>(glm::clamp(blend_factor, 0.0f, 1.0f) * 255.0f);
                } else {
                    // Dry mid-elevation: Pure dirt
                    v.base_id = 0;
                    v.extra_id = 0;
                    v.blend = 0;
                }
            } else if (humidity >= 0.3f) {
                // Humid areas: Pure grass
                v.base_id = 1;
                v.extra_id = 1;    // Same as base
                v.blend = 0;
            } else if (humidity >= 0.2f && humidity < 0.3f) {
                // Transition: Dirt to Grass
                v.base_id = 0;     // Dirt base
                v.extra_id = 1;    // Grass overlay
                float blend_factor = (humidity - 0.2f) / 0.1f;
                // Add noise variation for more organic transitions
                blend_factor += (test - 0.5f) * 0.1f; // ±5% variation
                v.blend = static_cast<uint32_t>(glm::clamp(blend_factor, 0.0f, 1.0f) * 255.0f);
            } else if (humidity >= 0.15f && humidity < 0.2f) {
                // Pure dirt
                v.base_id = 0;
                v.extra_id = 0;
                v.blend = 0;
            } else {
                // Very dry: Transition to sparse
                v.base_id = 0;
                v.extra_id = 0;
                v.blend = 0;
            }

            // Add random scale and rotation for enhanced detiling
            v.scale = static_cast<uint32_t>(test * 7.0f) % 8;        // 0-7 scale values
            v.rotation = static_cast<uint32_t>(test * 16.0f) % 16;   // 0-15 rotation values

            // DEBUG: Uncomment to visualize blend values
            // v.blend = static_cast<uint32_t>(test * 255.0f); // Random blend for testing

            *(controls + int(y * chunk_size) + x) = v;

            // Generate color map data
            int color_idx = int(y * chunk_size + x) * 4;

            // RGB: Color tinting based on elevation and humidity
            // High elevation (rock): Cooler gray-blue tint
            // Medium elevation (dirt): Warm brown tint
            // Low/humid (grass): Green tint

            glm::vec3 color_tint(1.0f);  // Default neutral white
            float roughness_mod = 0.5f;   // Default neutral (0.5)

            if (elevation >= 0.75f) {
                // High altitude rock: Cool gray-blue tint, rougher
                color_tint = glm::vec3(0.95f, 0.97f, 1.0f);  // Slight blue tint
                roughness_mod = 0.4f;  // Slightly rougher
            } else if (humidity >= 0.3f) {
                // Humid grass areas: Vibrant green tint, smoother
                color_tint = glm::vec3(0.9f, 1.0f, 0.9f);  // Slight green tint
                roughness_mod = 0.55f;  // Slightly smoother (grass)
            } else if (humidity < 0.2f) {
                // Dry dirt areas: Warm earthy tint, rougher
                color_tint = glm::vec3(1.0f, 0.95f, 0.9f);  // Slight warm tint
                roughness_mod = 0.45f;  // Rougher (dry dirt)
            }

            // Add subtle noise variation to color and roughness
            float noise_factor = test * 0.1f - 0.05f;  // -0.05 to +0.05
            color_tint = glm::clamp(color_tint + glm::vec3(noise_factor), 0.0f, 1.0f);
            roughness_mod = glm::clamp(roughness_mod + noise_factor * 0.5f, 0.0f, 1.0f);

            // Write to color map (RGBA8 format)
            color_map_data[color_idx + 0] = static_cast<uint8_t>(color_tint.r * 255.0f);  // R
            color_map_data[color_idx + 1] = static_cast<uint8_t>(color_tint.g * 255.0f);  // G
            color_map_data[color_idx + 2] = static_cast<uint8_t>(color_tint.b * 255.0f);  // B
            color_map_data[color_idx + 3] = static_cast<uint8_t>(roughness_mod * 255.0f); // A
        }
    }

    scene.add(
        Instance::builder()
            .terrain_size(chunk_size)
            .vertex_spacing(1.0f)
            .height(elevations)
            .height_scale(40.0f)
            .mesh_size(16.0f)
            .mesh_lod_count(4)
            .control(controls)
            .albedo_map(TextureLoader::load(
                assets, {
                    "../../assets/textures/dirt.jpg",
                    "../../assets/textures/grass.jpg",
                    "../../assets/textures/rock.jpg"
                },
                TextureLoaderFlags(TextureLoaderFlags::Space::sRGB)
            ))
            .normal_map(TextureLoader::load(
                assets, {
                    "../../assets/textures/dirt_normal.jpg",
                    "../../assets/textures/grass_normal.jpg",
                    "../../assets/textures/rock_normal.jpg"
                }
            ))
            .color_map(Texture::builder()
                    .target(Texture::Type::Tex2D)
                    .mipmap(false)
                    .internal_format(Texture::InternalFormat::RGBA8)
                    .size({static_cast<uint32_t>(chunk_size), static_cast<uint32_t>(chunk_size)})
                    .format(Texture::Format::RGBA)
                    .data_type(Texture::DataType::UnsignedByte)
                    .wrap_r(Texture::Wrap::ClampToEdge)
                    .wrap_s(Texture::Wrap::ClampToEdge)
                    .wrap_t(Texture::Wrap::ClampToEdge)
                    .min_filter(Texture::Filter::Linear)
                    .mag_filter(Texture::Filter::Linear)
                    .data(color_map_data)
                    .build()
                )
            .asTerrain(assets)
    );

    auto tessellated_plane = std::make_shared<Plane>();

    scene.add(Instance::builder()
        .model(tessellated_plane)
        .position({256.0f, 30.0f, 256.0f})
        .scale(glm::vec3{8.0f})
            .rotation(glm::vec3{M_PI_2, M_PI_2 * 3, 0.0f})
        .material(assets.materials.at("rock"))
        .build()
    );

    scene.add(Instance::builder()
    .model(assets.models.at("sphere"))
    .position({256.0f, 25.0f, 256.0f})
    .scale(glm::vec3{1.0f})
    .material(assets.materials.at("rock"))
    .build()
);

    delete[] elevations;
    delete[] controls;
    delete[] color_map_data;
}

void LimitlessMaterials::Scene::update(Context& context, const Camera& camera) {
    scene.update(camera);
}