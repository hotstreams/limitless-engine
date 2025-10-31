#include <limitless/instances/terrain_instance.hpp>
#include <limitless/ms/material_builder.hpp>
#include <limitless/util/geoclipmap.hpp>

#include <random>

#include <utility>
#include <iostream>

using namespace Limitless;
using namespace Limitless::ms;

void TerrainInstance::update(const Camera &camera) {
    // float camera_height = camera.getPosition().y;
    // float height_factor = glm::clamp((camera_height - lod_height_min) / (lod_height_max - lod_height_min), 0.0f, 1.0f);
    // // Use a power function to make the LOD change more slowly at low heights and faster at high heights.
    // height_factor = glm::pow(height_factor, 2.0f); // Example: quadratic
    // float lod_scale = glm::mix(lod_scale_min, lod_scale_max, height_factor) * lod_height_scale;
    //
    // static float last = original_spacing;
    // float current = original_spacing * lod_scale;
    // if (fabs(current - last) >= 1.0f)
    // {
    //     last = current;
    //     vertex_spacing = current;
    // }
    //
    // std::cout << "lod " << vertex_spacing << std::endl;

    snap(camera);

    mesh.cross->update(camera);

    mesh.seams->update(camera);
    mesh.trims->update(camera);
    mesh.fillers->update(camera);
    mesh.tiles->update(camera);

    const auto range = glm::vec2(height_scale * 0.5f, height_scale);
    const auto margin = 0.0f;

    mesh.cross->bounding_box.center.y = range.x - margin;
    mesh.cross->bounding_box.size.y = range.y + margin * 2.0f;


    for (const auto &item: mesh.seams->getInstances()) {
        item->bounding_box.center =
                glm::vec4{item->getPosition().x, item->getPosition().y + range.x - margin, item->getPosition().z, 1.0f} +
                glm::vec4{item->getAbstractModel().getBoundingBox().center, 1.0f} * item->getFinalMatrix();

        item->bounding_box.size =
                glm::vec4{item->getAbstractModel().getBoundingBox().size.x,
                          range.y + margin * 2.0f,
                          item->getAbstractModel().getBoundingBox().size.z,
                          1.0f} * item->getFinalMatrix();

        item->bounding_box.size = glm::abs(item->bounding_box.size);
    }

    for (const auto &item: mesh.trims->getInstances()) {
        item->bounding_box.center =
                glm::vec4{item->getPosition().x, item->getPosition().y + range.x - margin, item->getPosition().z, 1.0f} +
                glm::vec4{item->getAbstractModel().getBoundingBox().center, 1.0f} * item->getFinalMatrix();

        item->bounding_box.size =
                glm::vec4{item->getAbstractModel().getBoundingBox().size.x,
                          range.y + margin * 2.0f,
                          item->getAbstractModel().getBoundingBox().size.z,
                          1.0f} * item->getFinalMatrix();

        item->bounding_box.size = glm::abs(item->bounding_box.size);
    }

    for (const auto &item: mesh.fillers->getInstances()) {
        item->bounding_box.center =
                glm::vec4{item->getPosition().x, item->getPosition().y + range.x - margin, item->getPosition().z, 1.0f} +
                glm::vec4{item->getAbstractModel().getBoundingBox().center, 1.0f} * item->getFinalMatrix();

        item->bounding_box.size =
                glm::vec4{item->getAbstractModel().getBoundingBox().size.x,
                          range.y + margin * 2.0f,
                          item->getAbstractModel().getBoundingBox().size.z,
                          1.0f} * item->getFinalMatrix();

        item->bounding_box.size = glm::abs(item->bounding_box.size);
    }

    for (const auto &item: mesh.tiles->getInstances()) {
        item->bounding_box.center =
                glm::vec4{item->getPosition().x, item->getPosition().y + range.x - margin, item->getPosition().z, 1.0f} +
                glm::vec4{item->getAbstractModel().getBoundingBox().center, 1.0f} * item->getFinalMatrix();

        item->bounding_box.size =
                glm::vec4{item->getAbstractModel().getBoundingBox().size.x,
                          range.y + margin * 2.0f,
                          item->getAbstractModel().getBoundingBox().size.z,
                          1.0f} * item->getFinalMatrix();

        item->bounding_box.size = glm::abs(item->bounding_box.size);
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

                mesh.tiles->getInstances()[tile]->setPosition(tile_tl);
                mesh.tiles->getInstances()[tile]->setScale(glm::vec3(scale, 1.f, scale));

                tile++;
            }
        }

        {
            mesh.fillers->getInstances()[l]->setPosition(snapped_pos);
            mesh.fillers->getInstances()[l]->setScale(glm::vec3(scale, 1.f, scale));
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

                mesh.trims->getInstances()[edge]->setPosition(tile_center);
                mesh.trims->getInstances()[edge]->setScale(glm::vec3(scale, 1.f, scale));
                mesh.trims->getInstances()[edge]->setRotation(glm::vec3(0.f, -angle, 0.f));
            }

            {
                glm::vec3 next_base = next_snapped_pos - glm::vec3(float(mesh_size << (l + 1)), 0.f, float(mesh_size << (l + 1))) * vertex_spacing;

                mesh.seams->getInstances()[edge]->setPosition(next_base);
                mesh.seams->getInstances()[edge]->setScale(glm::vec3(scale, 1.f, scale));
            }
            edge++;
        }
    }
}

void TerrainInstance::initializeMesh(Assets& assets) {
    std::vector<std::shared_ptr<AbstractMesh>> meshes = GeoClipMap::generate(mesh_size, mesh_lod_count);

    std::vector<std::shared_ptr<ms::Material>> materials;
    materials.reserve(meshes.size());

    auto terrain_material = Material::builder()
            .name("terrain")

            .color(glm::vec4(1.0f))

            .shading(Shading::Lit)
            .models({InstanceType::Instanced, InstanceType::Model, InstanceType::Terrain})

            .normal_map()
            .orm_map()

            .custom("terrain_control_texture", control)
            .custom("terrain_albedo_texture", albedo)
            .custom("terrain_normal_texture", normal)
            .custom("terrain_height_texture", height_map)
            .custom("terrain_color_map", color_map) 

            .custom("terrain_size", terrain_size)
            .custom("terrain_texel_size", 1.0f / terrain_size)
            .custom("terrain_vertex_spacing", vertex_spacing)
            .custom("terrain_vertex_density", 1.0f / vertex_spacing)
            .custom("terrain_height_scale", height_scale)

            .custom("terrain_texture_uv_scale_array", texture_uv_scale)
            .custom("terrain_texture_color_map", color_map)
            .custom("terrain_texture_normal_depth_array", texture_normal_depth)
            .custom("terrain_texture_detile_array", texture_detile)

            .custom("enable_tile_bilerp", enable_tile_bilerp)
            .custom("normal_bilerp_multiplier", normal_bilerp_multiplier)
            .custom("tile_bilerp_multiplier", tile_bilerp_multiplier)

            .custom("bias_distance", bias_distance)
            .custom("mipmap_bias", mipmap_bias)
            .custom("depth_blur", depth_blur)

            .custom("blend_sharpness", blend_sharpness)

            .global_vertex(R"(
                #include "../terrain/terrain.glsl"
            )")
            .global_fragment(R"(
                #include "../terrain/terrain_surface.glsl"
            )")

            .vertex(R"(
                vec2 vertex_transformed = (getModelTransform() * vec4(vertex_position, 1.0)).xz;
                vec2 terrain_texel_uv = getTerrainTexelUV(vertex_transformed);
                vec2 terrain_uv = getTerrainUV(terrain_texel_uv);

                vertex_position.y = getTerrainHeight(terrain_uv);
            )")

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

    mesh.fillers = std::make_shared<InstancedInstance>();
    mesh.seams = std::make_shared<InstancedInstance>();
    mesh.tiles = std::make_shared<InstancedInstance>();
    mesh.trims = std::make_shared<InstancedInstance>();

    for (int l = 0; l < mesh_lod_count; l++) {
        for (int x = 0; x < 4; x++) {
            for (int y = 0; y < 4; y++) {
                if (l != 0 && (x == 1 || x == 2) && (y == 1 || y == 2)) {
                    continue;
                }
                mesh.tiles->add(std::make_shared<ModelInstance>(InstanceType::Terrain, tile_model, glm::vec3(0.0f)));
            }
        }

        mesh.fillers->add(std::make_shared<ModelInstance>(InstanceType::Terrain, filler_model, glm::vec3(0.0f)));

        if (l != mesh_lod_count - 1) {
            mesh.trims->add(std::make_shared<ModelInstance>(InstanceType::Terrain, trim_model, glm::vec3(0.0f)));
            mesh.seams->add(std::make_shared<ModelInstance>(InstanceType::Terrain, seam_model, glm::vec3(0.0f)));
        }
    }
}

TerrainInstance::TerrainInstance(
    std::shared_ptr<Texture> height_map,
    std::shared_ptr<Texture> control_map,
    std::shared_ptr<Texture> albedo_map,
    std::shared_ptr<Texture> normal_map,
    std::shared_ptr<Texture> color_map
)
    : Instance(InstanceType::Terrain, glm::vec3{0.0f})
    , height_map(std::move(height_map))
    , control(std::move(control_map))
    , albedo(std::move(albedo_map))
    , normal(std::move(normal_map))
    , color_map(std::move(color_map))
    , texture_uv_scale(MAX_TEXTURES, 1.0f)
    , texture_normal_depth(MAX_TEXTURES, 1.0f)
    , texture_detile(MAX_TEXTURES, glm::vec2{1.0f})
{

}

void TerrainInstance::setHeightMap(const std::shared_ptr<Texture>& height) {
    height_map = height;
}

void TerrainInstance::setControlMap(const std::shared_ptr<Texture>& texture) {
    control = texture;
}

void TerrainInstance::setAlbedoMap(const std::shared_ptr<Texture>& texture) {
    albedo = texture;
}

void TerrainInstance::setNormalMap(const std::shared_ptr<Texture>& texture) {
    normal = texture;
}

void TerrainInstance::setColorMap(const std::shared_ptr<Texture>& texture) {
    color_map = texture;
}

void TerrainInstance::updateHeight(const void* data) const {
    height_map->subImage(0, glm::uvec2{0}, glm::uvec2(terrain_size), data);
}

void TerrainInstance::updateHeight(glm::uvec2 offset, glm::uvec2 size, const void* data) const {
    height_map->subImage(0, offset, size, data);
}

void TerrainInstance::updateControl(const void *data) const {
    control->subImage(0, glm::uvec2{0}, glm::uvec2(terrain_size), data);
}

void TerrainInstance::updateControl(glm::uvec2 offset, glm::uvec2 size, const void *data) const {
    control->subImage(0, offset, size, data);
}

void TerrainInstance::setVertexSpacing(float vertexSpacing) {
    vertex_spacing = vertexSpacing;
}

void TerrainInstance::setMeshSize(int meshSize) {
    mesh_size = meshSize;
}

void TerrainInstance::setMeshLodCount(int meshLodCount) {
    mesh_lod_count = meshLodCount;
}

void TerrainInstance::setHeightScale(float height) {
    height_scale = height;
}

void TerrainInstance::setTerrainSize(float size) {
    terrain_size = size;
}

void TerrainInstance::enableTileBilerp()
{
    enable_tile_bilerp = 1;
}

void TerrainInstance::disableTileBilerp()
{
    enable_tile_bilerp = 0;
}

void TerrainInstance::setTileBilerp(bool bilerp)
{
    enable_tile_bilerp = bilerp;
}

void TerrainInstance::setNormalBilerpMultiplier(float multiplier)
{
    normal_bilerp_multiplier = multiplier;
}

void TerrainInstance::setTileBilerpMultiplier(float multiplier)
{
    tile_bilerp_multiplier = multiplier;
}

void TerrainInstance::setTextureUVScales(const std::vector<float>& scales) {
    // Update all texture scales at once
    // This would need to be implemented based on your material system
}


void TerrainInstance::setTextureColors(const std::vector<glm::vec4>& colors) {
    // Update all texture colors at once
    // This would need to be implemented based on your material system
}

void TerrainInstance::setTextureNormalDepths(const std::vector<float>& normal_depths) {
    // Update all texture normal depths at once
    // This would need to be implemented based on your material system
}

void TerrainInstance::setTextureDetiles(const std::vector<glm::vec2>& normal_depths)
{

}
