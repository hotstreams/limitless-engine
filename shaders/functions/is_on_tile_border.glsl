bool is_on_tile_border(vec3 pos, vec3 tile_size, float border_width) {
    vec3 tilePos = mod(pos, tile_size);
    return tilePos.x < border_width || tilePos.x > tile_size.x - border_width || tilePos.z < border_width || tilePos.z > tile_size.z - border_width;
}