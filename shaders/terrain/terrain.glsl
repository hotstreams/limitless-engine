void calculateTerrain(MaterialContext mctx) {
    vec3 diffuse = texture(_terrain_diffuse_texture, vec3(getVertexUV1(), getVertexTileCurrent())).rgb;
    vec3 normal = texture(_terrain_normal_texture, vec3(getVertexUV1(), getVertexTileCurrent())).xyz;
    vec3 orm = texture(_terrain_orm_texture, vec3(getVertexUV1(), getVertexTileCurrent())).rgb;

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
            // fetch adjacent type (//TODO: fetch cache?)
            vec3 adjacent_diffuse = texture(_terrain_diffuse_texture, vec3(getVertexUV1(), types[i])).rgb;
            vec3 adjacent_normal = texture(_terrain_normal_texture, vec3(getVertexUV1(), types[i])).xyz;
            vec3 adjacent_orm = texture(_terrain_orm_texture, vec3(getVertexUV1(), types[i])).rgb;

            // mix
            diffuse = mix(diffuse, adjacent_diffuse, (1.0 - vectors[i]));
            normal = mix(normal, adjacent_normal, (1.0 - vectors[i]));
            orm = mix(orm, adjacent_orm, (1.0 - vectors[i]));
        }
    }

    // store result
    mctx.diffuse.rgb = diffuse;
    mctx.normal = normalize(normal);
    mctx.ao = orm.r;
    mctx.roughness = orm.g;
    mctx.metallic =orm.b;
}