float MacroVariation() {
    vec2 uv = getVertexPosition().xz;
    vec2 uv1 = uv * variation1;
    vec2 uv2 = uv * variation2;
    vec2 uv3 = uv * variation3;
    float r1 = texture(_terrain_variation, uv1).r + 0.5;
    float r2 = texture(_terrain_variation, uv2).r + 0.5;
    float r3 = texture(_terrain_variation, uv3).r + 0.5;
    return r1 * r2 * r3;
}

vec3 MacroContrast(float variation) {
    return mix(variation_contrast, vec3(1.0), vec3(variation));
}

void calculateTerrain(inout MaterialContext mctx) {
    vec2 uv2 = getVertexUV1() * 4.0;
    vec3 variation = MacroContrast(MacroVariation());
    vec3 diffuse = texture(_terrain_diffuse_texture, vec3(uv2, getVertexTileCurrent())).rgb * vec3(1.0, 2.0, 1.0);
    vec3 normal = texture(_terrain_normal_texture, vec3(uv2, getVertexTileCurrent())).xyz;
    vec3 orm = texture(_terrain_orm_texture, vec3(uv2, getVertexTileCurrent())).rgb;

    vec2 uv = getVertexUV();

    float vectors[8] = {
        smoothstep(0.0, 1.0, length(uv)),
        smoothstep(0.0, 1.0, uv.y),
        smoothstep(0.0, 1.0, length(vec2(1.0 - uv.x, uv.y))),
        smoothstep(0.0, 1.0, uv.x),
        smoothstep(0.0, 1.0, 1.0 - uv.x),
        smoothstep(0.0, 1.0, length(vec2(uv.x, 1.0 - uv.y))),
        smoothstep(0.0, 1.0, 1.0 - uv.y),
        smoothstep(0.0, 1.0, length(vec2(1.0 - uv.x, 1.0 - uv.y)))
    };

    uint mask[8];
    unpack8x4(getVertexColor(), mask);

    uint types[8];
    unpack8x4(getVertexTileType(), types);

    // iterate for all adjacent tiles
    for (uint i = 0u; i < 8u; ++i) {
        // shoud we blend
        if (mask[i] == 1u) {
            // fetch adjacent type
            vec3 adjacent_diffuse = texture(_terrain_diffuse_texture, vec3(uv2, types[i])).rgb;
            vec3 adjacent_normal = texture(_terrain_normal_texture, vec3(uv2, types[i])).xyz;
            vec3 adjacent_orm = texture(_terrain_orm_texture, vec3(uv2, types[i])).rgb;

            // mix
            diffuse = mix(diffuse, adjacent_diffuse, (1.0 - vectors[i]));
            normal = mix(normal, adjacent_normal, (1.0 - vectors[i]));
            orm = mix(orm, adjacent_orm, (1.0 - vectors[i]));
        }
    }

    diffuse *= variation;

    // store result
    mctx.color.rgb = diffuse;
    mctx.normal = normalize(normal);
    mctx.ao = orm.r;
    mctx.roughness = orm.g;
    mctx.metallic =orm.b;
}