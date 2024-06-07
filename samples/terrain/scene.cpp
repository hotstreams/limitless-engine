#include "scene.hpp"

#include <limitless/scene.hpp>
#include <limitless/instances/instanced_instance.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/lighting/light.hpp>
#include <random>
#include <limitless/core/vertex.hpp>

using namespace LimitlessMaterials;
using namespace Limitless;

LimitlessMaterials::Scene::Scene(Limitless::Context& ctx, Limitless::Assets& assets)
    : scene(ctx)
    , assets {assets} {
}

void LimitlessMaterials::Scene::update(Limitless::Context& context, const Limitless::Camera& camera) {
    scene.update(context, camera);
}

void LimitlessMaterials::Scene::createTerrain() {
    const auto dims   = {256, 256};
    const auto width  = 256;
    const auto height = 256;

    auto vertices         = std::vector<Limitless::VertexTerrain>();
    auto indices          = std::vector<uint32_t>();
    auto triangle_normals = std::vector<glm::vec3>();

    uint32_t i = 0;
    for (auto y = 0; y < height; ++y) {
        for (auto x = 0; x < width; ++x) {
            const auto chunk_tiles = 8;
            const auto chunk_size  = 1.0f / chunk_tiles;

            const auto bottom_left =
                    glm::vec2 {chunk_size * (x % chunk_tiles), chunk_size * (y % chunk_tiles)};
            const auto bottom_right = glm::vec2 {
                    chunk_size * (x % chunk_tiles) + chunk_size, chunk_size * (y % chunk_tiles)};
            const auto top_left = glm::vec2 {
                    chunk_size * (x % chunk_tiles), chunk_size * (y % chunk_tiles) + chunk_size};
            const auto top_right = glm::vec2 {
                    chunk_size * (x % chunk_tiles) + chunk_size,
                    chunk_size * (y % chunk_tiles) + chunk_size};

            const auto uvs = std::array {bottom_left, bottom_right, top_right, top_left};

            const auto tile_tl = fetchTile(map, Location(x, y));
            const auto tile_tr = fetchTile(map, Location(x + 1, y));
            const auto tile_bl = fetchTile(map, Location(x, y + 1));
            const auto tile_br = fetchTile(map, Location(x + 1, y + 1));

            glm::ivec4 type {0};
            int32_t current {(int32_t)tile_tl.getTerrain()};

            std::array<uint32_t, 8> blendMask {0};
            std::array<uint32_t, 8> types {0};
            std::array locs = {
                    Location(x - 1, y - 1),
                    Location(x, y - 1),
                    Location(x + 1, y - 1),

                    Location(x - 1, y),
                    Location(x + 1, y),

                    Location(x - 1, y + 1),
                    Location(x, y + 1),
                    Location(x + 1, y + 1),
            };

        for (size_t j = 0; j < locs.size(); ++j) {
            auto another_tile = fetchTile(map, locs[j]);
            if (tile_tl.getTerrain() != another_tile.getTerrain()
                && isBlendNeeded(tile_tl, another_tile)) {
                blendMask[j] = 1u;
                types[j]     = (uint32_t)another_tile.getTerrain();
            }
        }

        uint32_t packed_mask  = pack8to1(blendMask);
        uint32_t packed_types = pack8to1(types);

        const auto p0 =
                glm::vec3 {tile_size * x, modelPosYFrom(gameElevationFrom(tile_tl)), tile_size * y};
        const auto p1 = glm::vec3 {
                tile_size * (x + 1), modelPosYFrom(gameElevationFrom(tile_tr)), tile_size * y};
        const auto p2 = glm::vec3 {
                tile_size * (x + 1), modelPosYFrom(gameElevationFrom(tile_br)), tile_size * (y + 1)};
        const auto p3 = glm::vec3 {
                tile_size * x, modelPosYFrom(gameElevationFrom(tile_bl)), tile_size * (y + 1)};

        const auto n1 = glm::normalize(glm::cross(p2 - p0, p1 - p0));
        const auto n2 = glm::normalize(glm::cross(p3 - p0, p2 - p0));

        triangle_normals.emplace_back(n1);
        triangle_normals.emplace_back(n2);

        vertices.emplace_back(
                p0,
                glm::vec3 {},
                glm::vec3 {0.f},
                glm::vec2 {0, 0},
                uvs[0],
                uvs[0],
                uvs[0],
                current,
                packed_mask,
                packed_types
        );
        vertices.emplace_back(
                p1,
                glm::vec3 {},
                glm::vec3 {0.f},
                glm::vec2 {(0 + 1), 0},
                uvs[1],
                uvs[1],
                uvs[1],
                current,
                packed_mask,
                packed_types
        );
        vertices.emplace_back(
                p2,
                glm::vec3 {},
                glm::vec3 {0.f},
                glm::vec2 {(0 + 1), (0 + 1)},
                uvs[2],
                uvs[2],
                uvs[2],
                current,
                packed_mask,
                packed_types
        );
        vertices.emplace_back(
                p3,
                glm::vec3 {},
                glm::vec3 {0.f},
                glm::vec2 {0, (0 + 1)},
                uvs[3],
                uvs[3],
                uvs[3],
                current,
                packed_mask,
                packed_types
        );

        indices.emplace_back(i);
        indices.emplace_back(i + 2);
        indices.emplace_back(i + 1);

        indices.emplace_back(i);
        indices.emplace_back(i + 3);
        indices.emplace_back(i + 2);

        i += 4;
    }
}

calculateSmoothNormals(dims, triangle_normals, vertices);

Limitless::calculateTangentSpaceTriangle(vertices, indices);

auto mesh = std::make_shared<Limitless::Mesh>(
        std::make_unique<Limitless::IndexedVertexStream<Limitless::VertexTerrain>>(
                std::move(vertices),
                std::move(indices),
                Limitless::VertexStreamUsage::Static,
                Limitless::VertexStreamDraw::Triangles
        ),
        "terrain"
);

Limitless::ms::Material::Builder builder;

auto material =
        builder.name("terrain")
                .color(glm::vec4 {1.0f})
                .shading(Limitless::ms::Shading::Lit)
                .model(Limitless::InstanceType::Terrain)

                .diffuse(Limitless::TextureLoader::load(assets, "dirt.png"))
                .normal(Limitless::TextureLoader::load(assets, "dirt_norm.png"))
                .orm(Limitless::TextureLoader::load(assets, "dirt_orm.png"))

                .custom("rock", Limitless::TextureLoader::load(assets, "rock.png"))
                .custom("rock_norm", Limitless::TextureLoader::load(assets, "rock_norm.png"))
                .custom("rock_orm", Limitless::TextureLoader::load(assets, "rock_orm.png"))

                .custom("grass", Limitless::TextureLoader::load(assets, "grass/grass_color.jpg"))
                .custom("grass_norm", Limitless::TextureLoader::load(assets, "grass/grass_normal.jpg"))
                .custom("grass_orm", Limitless::TextureLoader::load(assets, "grass_orm.png"))

                        //                        .custom("noise", Limitless::TextureLoader::load(assets,
                        //                        "noise.png"))

                .global(""
                        "void unpack8x4(uint value, out uint unpacked[8]) {\n"
                        "    unpacked[0] = (value      ) & 0x0Fu;\n"
                        "    unpacked[1] = (value >>  4u) & 0x0Fu;\n"
                        "    unpacked[2] = (value >>  8u) & 0x0Fu;\n"
                        "    unpacked[3] = (value >> 12u) & 0x0Fu;\n"
                        "    unpacked[4] = (value >> 16u) & 0x0Fu;\n"
                        "    unpacked[5] = (value >> 20u) & 0x0Fu;\n"
                        "    unpacked[6] = (value >> 24u) & 0x0Fu;\n"
                        "    unpacked[7] = (value >> 28u) & 0x0Fu;\n"
                        "}"
                        //                         ""
                        //                         "  float sumV(vec3 v) { return v.x + v.y + v.z; }"
                        //                         ""
                        //                         "vec4 untiling(sampler2D tex, vec2 uv) {"
                        //                         "      float k = texture(noise, 0.005 * uv).x;"
                        //                         ""
                        //                         "      float index = k * 8.0;"
                        //                         "      float i = floor(index);"
                        //                         "      float f = fract(index);"
                        //                         ""
                        //                         "      vec2 offa = sin(vec2(3.0, 7.0) * (i + 0.0));"
                        //                         "      vec2 offb = sin(vec2(3.0, 7.0) * (i + 1.0));"
                        //                         ""
                        //                         "      vec2 dx = dFdx(uv);"
                        //                         "      vec2 dy = dFdy(uv);"
                        //                         ""
                        //                         "      vec3 cola = textureGrad(tex, uv + offa, dx,
                        //                         dy).xyz;" "      vec3 colb = textureGrad(tex, uv +
                        //                         offb, dx, dy).xyz;"
                        //                         ""
                        //                         "      return vec4(mix(cola, colb, smoothstep(0.2,
                        //                         0.8, f - 0.1 * sumV(cola-colb))), 1.0);"
                        //                         "}
                )
                .fragment(
                        ""
                        "vec4 d = getMaterialDiffuse(getVertexUV1());"
                        "vec4 r = texture(rock, getVertexUV1());"
                        "vec4 g = texture(grass, getVertexUV1());"

                        "vec3 dn = getMaterialNormal(getVertexUV1());"
                        "vec3 rn = texture(rock_norm, getVertexUV1()).xyz;"
                        "vec3 gn = texture(grass_norm, getVertexUV1()).xyz;"
                        ""
                        "vec3 dorm = texture(_material_orm_texture, getVertexUV1()).rgb;"
                        "vec3 rorm = texture(rock_orm, getVertexUV1()).rgb;"
                        "vec3 gorm = texture(grass_orm, getVertexUV1()).rgb;"
                        ""
                        "vec3 orm;"
                        "if (getVertexTileCurrent() == 0u) {"
                        "     orm = vec3(mctx.ao, mctx.roughness, mctx.metallic);"
                        " }"

                        "if (getVertexTileCurrent() == 1u) {"
                        "     mctx.color = g;"
                        "     mctx.normal = texture(grass_norm, getVertexUV1()).xyz;"
                        "     orm = texture(grass_orm, getVertexUV1()).rgb;"
                        "}"
                        "if (getVertexTileCurrent() == 2u) {"
                        "     mctx.color = r;"
                        "     mctx.normal = texture(rock_norm, getVertexUV1()).xyz;"
                        "     orm = texture(rock_orm, getVertexUV1()).rgb;"
                        "}"

                        "vec3 blend_color = vec3(0.0);"

                        "vec2 uv = getVertexUV();"

                        "      float vectors[8] = {"
                        "          smoothstep(0.0, 1.0, length(uv)),"
                        "          smoothstep(0.0, 1.0, uv.y),"
                        "          smoothstep(0.0, 1.0, length(vec2(1.0 - uv.x, uv.y))),"
                        "          smoothstep(0.0, 1.0, uv.x),"
                        "          smoothstep(0.0, 1.0, 1.0 - uv.x),"
                        "          smoothstep(0.0, 1.0, length(vec2(uv.x, 1.0 - uv.y))),"
                        "          smoothstep(0.0, 1.0, 1.0 - uv.y),"
                        "          smoothstep(0.0, 1.0, length(vec2(1.0 - uv.x, 1.0 - uv.y)))"
                        "      };"
                        ""
                        "      uint mask[8];"
                        "      unpack8x4(getVertexColor(), mask);"
                        ""
                        "      uint types[8];"
                        "      unpack8x4(getVertexTileType(), types);"
                        ""
                        "      "
                        "for (uint i = 0u; i < 8u; ++i) {"
                        "     if (mask[i] == 1u) {"
                        "         switch (types[i]) {"
                        "             case 0u:"
                        "                 mctx.color.rgb = mix(mctx.color.rgb, d.rgb, (1.0 - vectors[i]));"
                        "                 mctx.normal = mix(mctx.normal, dn.rgb, (1.0 - vectors[i]));"
                        "                 orm = mix(orm, dorm.rgb, (1.0 - vectors[i]));"
                        "                 break;"
                        "             case 1u:"
                        "                 mctx.color.rgb = mix(mctx.color.rgb, g.rgb, (1.0 - vectors[i]));"
                        "                 mctx.normal = mix(mctx.normal, gn.rgb, (1.0 - vectors[i])); "
                        "                 orm = mix(orm, gorm.rgb, (1.0 - vectors[i])); "
                        "                 break;"
                        "             case 2u:"
                        "                 mctx.color.rgb = mix(mctx.color.rgb, r.rgb, (1.0 - vectors[i]));"
                        "                 mctx.normal = mix(mctx.normal, rn.rgb, (1.0 - vectors[i]));"
                        "                 orm = mix(orm, rorm.rgb, (1.0 - vectors[i]));"
                        "                 break;"
                        "         }"
                        "     } "
                        "}"

                        ""
                        "mctx.normal = normalize(mctx.normal);"
                        ""
                        "mctx.ao = orm.r;"
                        "mctx.roughness = orm.g;"
                        "mctx.metallic = orm.b;"
                )
                .build(assets);
}
