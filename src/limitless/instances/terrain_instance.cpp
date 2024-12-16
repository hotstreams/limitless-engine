#include <limitless/instances/terrain_instance.hpp>
#include <limitless/ms/material_builder.hpp>
#include <random>

#include <limitless/util/noise.hpp>
#include <utility>

using namespace Limitless;
using namespace Limitless::ms;

void TerrainInstance::update(const Camera &camera) {
    snap(camera);
    
    mesh.cross->update(camera);

    for (auto &item: mesh.seams) {
        item->update(camera);
    }

    for (auto &item: mesh.trims) {
        item->update(camera);
    }

    for (auto &item: mesh.fillers) {
        item->update(camera);
    }

    for (auto &item: mesh.tiles) {
        item->update(camera);
    }
}

void TerrainInstance::snap(const Camera& p_cam_pos) {
    glm::vec3 cam_pos = p_cam_pos.getPosition();
    cam_pos.y = 0;

    glm::vec3 snapped_pos = glm::floor(cam_pos / vertex_spacing) * vertex_spacing;

    mesh.cross->setPosition(snapped_pos);
    mesh.cross->setScale(glm::vec3(vertex_spacing, 1, vertex_spacing));

    int edge = 0;
    int tile = 0;

    for (int l = 0; l < mesh_lod_count; l++) {
        float scale = float(1 << l) * vertex_spacing;
        snapped_pos = glm::floor(cam_pos / scale) * scale;
        glm::vec3 tile_size = glm::vec3(float(mesh_size << l), 0, float(mesh_size << l)) * vertex_spacing;
        glm::vec3 base = snapped_pos - glm::vec3(float(mesh_size << (l + 1)), 0.f, float(mesh_size << (l + 1))) * vertex_spacing;

        // Position tiles
        for (int x = 0; x < 4; x++) {
            for (int y = 0; y < 4; y++) {
                if (l != 0 && (x == 1 || x == 2) && (y == 1 || y == 2)) {
                    continue;
                }

                glm::vec3 fill = glm::vec3(x >= 2 ? 1.f : 0.f, 0.f, y >= 2 ? 1.f : 0.f) * scale;
                glm::vec3 tile_tl = base + glm::vec3(x, 0.f, y) * tile_size + fill;
                glm::vec3 tile_br = tile_tl + tile_size;

                mesh.tiles[tile]->setPosition(tile_tl);
                mesh.tiles[tile]->setScale(glm::vec3(scale, 1.f, scale));

                tile++;
            }
        }

        {
            mesh.fillers[l]->setPosition(snapped_pos);
            mesh.fillers[l]->setScale(glm::vec3(scale, 1.f, scale));
        }

        if (l != mesh_lod_count - 1) {
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

                mesh.trims[edge]->setPosition(tile_center);
                mesh.trims[edge]->setScale(glm::vec3(scale, 1.f, scale));
                mesh.trims[edge]->setRotation(glm::vec3(0.f, -angle, 0.f));
            }

            // Position seams
            {
                glm::vec3 next_base = next_snapped_pos - glm::vec3(float(mesh_size << (l + 1)), 0.f, float(mesh_size << (l + 1))) * vertex_spacing;

                mesh.seams[edge]->setPosition(next_base);
                mesh.seams[edge]->setScale(glm::vec3(scale, 1.f, scale));
            }
            edge++;
        }
    }
}

void TerrainInstance::initializeMesh(Assets& assets) {
    std::vector<std::shared_ptr<AbstractMesh>> meshes = GeoClipMap::generate(mesh_size, mesh_lod_count);

    std::vector<std::shared_ptr<ms::Material>> materials;
    materials.reserve(meshes.size());

    auto terrain_material = Limitless::ms::Material::builder()
            .name("terrain")
            .color(glm::vec4(1.0))
//            .two_sided(true)
            .shading(Limitless::ms::Shading::Lit)
//            .shading(Limitless::ms::Shading::Unlit)
            .model(Limitless::InstanceType::Terrain)

            .normal(orm)
//            .orm(noise)

            // textures
            .custom("terrain_control_texture", control)
            .custom("terrain_albedo_texture", albedo)
            .custom("terrain_normal_texture", normal)
            .custom("terrain_orm_texture", orm)
            .custom("terrain_noise_texture", noise)
            .custom("terrain_height_texture", height_map)

            // scalars
            .custom("terrain_size", terrain_size)
            .custom("terrain_texel_size", terrain_texel_size)
            .custom("terrain_texture_scale", 64.0f)
            .custom("terrain_vertex_spacing", vertex_spacing)
            .custom("terrain_vertex_density", vertex_density)
            .custom("terrain_height_scale", height_scale)
            .custom("terrain_noise1_scale", noise1_scale)
            .custom("terrain_noise2_scale", noise2_scale)
            .custom("terrain_noise2_angle", noise2_angle)
            .custom("terrain_noise2_offset", noise2_offset)
            .custom("terrain_noise3_scale", noise3_scale)
            .custom("terrain_macro_variation1", macro_variation1)
            .custom("terrain_macro_variation2", macro_variation2)

            // switches
            .custom("terrain_show_tiles", show_tiles)

            .global("#include \"../terrain/terrain.glsl\"\n")

            // custom vertex shader
            .vertex(R"(
                vec3 v_vertex = (getModelTransform() * vec4(vertex_position, 1.0)).xyz;

                vec2 UV = mod(v_vertex.xz, (terrain_size * terrain_vertex_spacing));
                UV = UV / (terrain_size * terrain_vertex_spacing);

                vertex_position.y = texture(terrain_height_texture, UV).r * terrain_height_scale;

                float u = texture(terrain_height_texture,UV + vec2(terrain_texel_size, 0)).r * terrain_height_scale - texture(terrain_height_texture,UV - vec2(terrain_texel_size, 0)).r * terrain_height_scale;
                float v = texture(terrain_height_texture,UV + vec2(0, terrain_texel_size)).r * terrain_height_scale - texture(terrain_height_texture,UV - vec2(0, terrain_texel_size)).r * terrain_height_scale;

                normal = normalize(vec3(u, 2.0 * terrain_vertex_spacing, v));
            )")

            // custom fragment shader
            .fragment(R"(
                calculateTerrain(mctx);
            )")
            .build(assets);

    for (const auto& _: meshes) {
        materials.emplace_back(terrain_material);
    }

    auto tile_model = std::make_shared<Model>(std::vector{meshes[0]}, std::vector{materials[0]}, "map");
    auto filler_model = std::make_shared<Model>(std::vector{meshes[1]}, std::vector{materials[1]}, "map");
    auto trim_model = std::make_shared<Model>(std::vector{meshes[2]}, std::vector{materials[2]}, "map");
    auto cross_model = std::make_shared<Model>(std::vector{meshes[3]}, std::vector{materials[3]}, "map");
    auto seam_model = std::make_shared<Model>(std::vector{meshes[4]}, std::vector{materials[4]}, "map");

    mesh.cross = std::make_shared<ModelInstance>(InstanceType::Terrain, std::move(cross_model), glm::vec3(0.0f));

    for (int l = 0; l < mesh_lod_count; l++) {
        for (int x = 0; x < 4; x++) {
            for (int y = 0; y < 4; y++) {
                if (l != 0 && (x == 1 || x == 2) && (y == 1 || y == 2)) {
                    continue;
                }
                mesh.tiles.emplace_back(std::make_shared<ModelInstance>(InstanceType::Terrain, tile_model, glm::vec3(0.0f)));
            }
        }

        mesh.fillers.emplace_back(std::make_shared<ModelInstance>(InstanceType::Terrain, filler_model, glm::vec3(0.0f)));

        if (l != mesh_lod_count - 1) {
            mesh.trims.emplace_back(std::make_shared<ModelInstance>(InstanceType::Terrain, trim_model, glm::vec3(0.0f)));
            mesh.seams.emplace_back(std::make_shared<ModelInstance>(InstanceType::Terrain, seam_model, glm::vec3(0.0f)));
        }
    }
}

TerrainInstance::TerrainInstance(
    std::shared_ptr<Texture> height_map,
    std::shared_ptr<Texture> control_map,
    std::shared_ptr<Texture> albedo_map,
    std::shared_ptr<Texture> normal_map,
    std::shared_ptr<Texture> orm_map,
    std::shared_ptr<Texture> noise
)
    : Instance(InstanceType::Terrain, glm::vec3{0.0f})
    , height_map(std::move(height_map))
    , control(std::move(control_map))
    , albedo(std::move(albedo_map))
    , normal(std::move(normal_map))
    , orm(std::move(orm_map))
    , noise(std::move(noise))

{

}

void TerrainInstance::setHeightMap(const std::shared_ptr<Texture>& height) {
    height_map = height;
}

void TerrainInstance::setControlMap(const std::shared_ptr<Texture>& texture) {
    control = texture;
}

void TerrainInstance::setNoise(const std::shared_ptr<Texture>& texture) {
    noise = texture;
}

void TerrainInstance::setAlbedoMap(const std::shared_ptr<Texture>& texture) {
    albedo = texture;
}

void TerrainInstance::setNormalMap(const std::shared_ptr<Texture>& texture) {
    normal = texture;
}

void TerrainInstance::setOrmMap(const std::shared_ptr<Texture>& texture) {
    orm = texture;
}

void TerrainInstance::updateHeight(const void* data) {
    height_map->subImage(0, glm::uvec2{0}, glm::uvec2(terrain_size), data);
}

void TerrainInstance::updateHeight(glm::uvec2 offset, glm::uvec2 size, const void* data) {
    height_map->subImage(0, offset, size, data);
}

void TerrainInstance::updateControl(const void *data) {
    control->subImage(0, glm::uvec2{0}, glm::uvec2(terrain_size), data);
}

void TerrainInstance::updateControl(glm::uvec2 offset, glm::uvec2 size, const void *data) {
    control->subImage(0, offset, size, data);
}

void TerrainInstance::setChunkSize(float chunkSize) {
    terrain_size = chunkSize;
}

void TerrainInstance::setVertexSpacing(float vertexSpacing) {
    vertex_spacing = vertexSpacing;
}

void TerrainInstance::setHeightScale(float heightScale) {
    height_scale = heightScale;
}

void TerrainInstance::setNoise1Scale(float noise1Scale) {
    noise1_scale = noise1Scale;
}

void TerrainInstance::setNoise2Scale(float noise2Scale) {
    noise2_scale = noise2Scale;
}

void TerrainInstance::setNoise2Angle(float noise2Angle) {
    noise2_angle = noise2Angle;
}

void TerrainInstance::setNoise2Offset(float noise2Offset) {
    noise2_offset = noise2Offset;
}

void TerrainInstance::setNoise3Scale(float noise3Scale) {
    noise3_scale = noise3Scale;
}

void TerrainInstance::setMacroVariation1(const glm::vec3 &macroVariation1) {
    macro_variation1 = macroVariation1;
}

void TerrainInstance::setMacroVariation2(const glm::vec3 &macroVariation2) {
    macro_variation2 = macroVariation2;
}

void TerrainInstance::setMeshSize(int meshSize) {
    mesh_size = meshSize;
}

void TerrainInstance::setMeshLodCount(int meshLodCount) {
    mesh_lod_count = meshLodCount;
}

std::shared_ptr<Texture> TerrainInstance::generateNormalMap() {
    return std::shared_ptr<Texture>();
}

void TerrainInstance::setGeneratedNormalMap(const std::shared_ptr<Texture>& normal_map) {
    generated_normal_map = normal_map;
}
