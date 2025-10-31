#define DEBUG_NONE 0
#define DEBUG_CHECKERED 1
#define DEBUG_GREY 2
#define DEBUG_HEIGHTMAP 3
#define DEBUG_CONTROL_TEXTURE 4
#define DEBUG_CONTROL_BLEND 5
#define DEBUG_CONTROL_ANGLE 6
#define DEBUG_CONTROL_SCALE 7
#define DEBUG_COLORMAP 8
#define DEBUG_ROUGHMAP 9
#define DEBUG_TEXTURE_HEIGHT 10
#define DEBUG_TEXTURE_NORMAL 11
#define DEBUG_TEXTURE_ROUGHNESS 12
#define DEBUG_VERTEX_NORMALS 13
#define DEBUG_BILERP 14
#define DEBUG_TEXTURE_IDS 15
#define DEBUG_TOTAL_WEIGHT 16

void applyDebugVisualization(int debug_mode, vec2 uv, uint control, inout MaterialContext mctx, const tile_data mat, bool bilerp, ivec2 texture_ids) {
    if (debug_mode == DEBUG_NONE) {
        return;
    }

    // DEBUG_CHECKERED - Show a checkered grid
    if (debug_mode == DEBUG_CHECKERED) {
        vec2 p = uv * 1.0;
        vec2 ddx = dFdx(p);
        vec2 ddy = dFdy(p);
        vec2 w = max(abs(ddx), abs(ddy)) + 0.01;
        vec2 i = 2.0 * (abs(fract((p - 0.5 * w) / 2.0) - 0.5) - abs(fract((p + 0.5 * w) / 2.0) - 0.5)) / w;
        mctx.color.xyz = vec3((0.5 - 0.5 * i.x * i.y) * 0.2 + 0.2);
        mctx.roughness = 0.7;
        mctx.metallic = 0.0;
        return;
    }

    // DEBUG_GREY - Show all grey
    if (debug_mode == DEBUG_GREY) {
        mctx.color.xyz = vec3(0.2);
        mctx.roughness = 0.7;
        mctx.metallic = 0.0;
        return;
    }

    // DEBUG_HEIGHTMAP - Show heightmap
    if (debug_mode == DEBUG_HEIGHTMAP) {
        float h = getVertexPosition().y;
        float factor = clamp((h - 0.0) / max(terrain_height_scale, 1e-6), 0.0, 1.0);
        mctx.color.xyz = vec3(smoothstep(0.0, 1.0, factor));
        mctx.roughness = 0.7;
        mctx.metallic = 0.0;
        return;
    }

    // DEBUG_CONTROL_TEXTURE - Show texture selection with colors
    if (debug_mode == DEBUG_CONTROL_TEXTURE) {
        vec3 t_colors[32];
        t_colors[0] = vec3(1.0, 0.0, 0.0);   // Red
        t_colors[1] = vec3(0.0, 1.0, 0.0);   // Green
        t_colors[2] = vec3(0.0, 0.0, 1.0);   // Blue
        t_colors[3] = vec3(1.0, 0.0, 1.0);   // Magenta
        t_colors[4] = vec3(0.0, 1.0, 1.0);   // Cyan
        t_colors[5] = vec3(1.0, 1.0, 0.0);   // Yellow
        t_colors[6] = vec3(0.2, 0.0, 0.0);   // Dark red
        t_colors[7] = vec3(0.0, 0.2, 0.0);   // Dark green
        t_colors[8] = vec3(0.0, 0.0, 0.35);  // Dark blue
        t_colors[9] = vec3(0.2, 0.0, 0.2);   // Dark magenta
        t_colors[10] = vec3(0.0, 0.2, 0.2);  // Dark cyan
        t_colors[11] = vec3(0.2, 0.2, 0.0);  // Dark yellow
        t_colors[12] = vec3(0.1, 0.0, 0.0);
        t_colors[13] = vec3(0.0, 0.1, 0.0);
        t_colors[14] = vec3(0.0, 0.0, 0.15);
        t_colors[15] = vec3(0.1, 0.0, 0.1);
        t_colors[16] = vec3(0.0, 0.1, 0.1);
        t_colors[17] = vec3(0.1, 0.1, 0.0);
        t_colors[18] = vec3(0.2, 0.05, 0.05);
        t_colors[19] = vec3(0.1, 0.3, 0.1);
        t_colors[20] = vec3(0.05, 0.05, 0.2);
        t_colors[21] = vec3(0.1, 0.05, 0.2);
        t_colors[22] = vec3(0.05, 0.15, 0.2);
        t_colors[23] = vec3(0.2, 0.2, 0.1);
        t_colors[24] = vec3(1.0);
        t_colors[25] = vec3(0.5);
        t_colors[26] = vec3(0.35);
        t_colors[27] = vec3(0.25);
        t_colors[28] = vec3(0.15);
        t_colors[29] = vec3(0.1);
        t_colors[30] = vec3(0.05);
        t_colors[31] = vec3(0.0125);

        vec3 ctrl_base = t_colors[DECODE_BASE_ID(control)];
        vec3 ctrl_over = t_colors[DECODE_EXTRA_ID(control)];
        float blend = DECODE_BLEND(control);
        
        // Show overlay texture as circle in center based on blend amount
        float base_over = (length(fract(uv) - 0.5) < fma(blend, 0.45, 0.1) ? 1.0 : 0.0);
        mctx.color.xyz = mix(ctrl_base, ctrl_over, base_over);
        mctx.roughness = 1.0;
        mctx.metallic = 0.0;
        return;
    }

    // DEBUG_CONTROL_BLEND - Show blend values
    if (debug_mode == DEBUG_CONTROL_BLEND) {
        mctx.color.xyz = vec3(DECODE_BLEND(control));
        mctx.roughness = 1.0;
        mctx.metallic = 0.0;
        return;
    }

    // DEBUG_CONTROL_ANGLE - Show texture rotation
    if (debug_mode == DEBUG_CONTROL_ANGLE) {
        uint angle = uint(DECODE_ROTATION(control) / 0.392699081698724);
        vec3 a_colors[16] = vec3[16](
            vec3(1., .2, .0), vec3(.8, 0., .2), vec3(.6, .0, .4), vec3(.4, .0, .6),
            vec3(.2, 0., .8), vec3(.1, .1, .8), vec3(0., .2, .8), vec3(0., .4, .6),
            vec3(0., .6, .4), vec3(0., .8, .2), vec3(0., 1., 0.), vec3(.2, 1., 0.),
            vec3(.4, 1., 0.), vec3(.6, 1., 0.), vec3(.8, .6, 0.), vec3(1., .4, 0.)
        );
        mctx.color.xyz = a_colors[angle];
        mctx.roughness = 1.0;
        mctx.metallic = 0.0;
        return;
    }

    // DEBUG_CONTROL_SCALE - Show texture scale
    if (debug_mode == DEBUG_CONTROL_SCALE) {
        // Reverse the scale formula to get the original scale index
        float scale_value = DECODE_SCALE(control);
        uint scale = uint((0.9 - scale_value) / 0.1) % 8u;
        vec3 s_colors[8] = vec3[8](
            vec3(.5, .5, .5), vec3(.675, .25, .375), vec3(.75, .125, .25), vec3(.875, .0, .125),
            vec3(1., 0., 0.), vec3(0., 0., 1.), vec3(.0, .166, .833), vec3(.166, .333, .666)
        );
        mctx.color.xyz = s_colors[scale];
        mctx.roughness = 1.0;
        mctx.metallic = 0.0;
        return;
    }

    // DEBUG_COLORMAP - Show color map
    if (debug_mode == DEBUG_COLORMAP) {
        vec2 vertex_position_xz = getVertexPosition().xz;
        vec2 terrain_texel_uv = getTerrainTexelUV(vertex_position_xz);
        vec2 terrain_texel_base = floor(terrain_texel_uv);
        ivec2 index = ivec2(terrain_texel_base);
        vec4 color = fetchColor(index);
        mctx.color.xyz = color.rgb;
        mctx.roughness = 0.7;
        mctx.metallic = 0.0;
        return;
    }

    // DEBUG_ROUGHMAP - Show roughness from color map alpha
    if (debug_mode == DEBUG_ROUGHMAP) {
        vec2 vertex_position_xz = getVertexPosition().xz;
        vec2 terrain_texel_uv = getTerrainTexelUV(vertex_position_xz);
        vec2 terrain_texel_base = floor(terrain_texel_uv);
        ivec2 index = ivec2(terrain_texel_base);
        vec4 color = fetchColor(index);
        mctx.color.xyz = vec3(color.a);
        mctx.roughness = 0.7;
        mctx.metallic = 0.0;
        return;
    }

    // DEBUG_TEXTURE_HEIGHT - Show height from textures
    if (debug_mode == DEBUG_TEXTURE_HEIGHT) {
        mctx.color.xyz = vec3(mat.albedo_height.a);
        mctx.roughness = 0.7;
        mctx.metallic = 0.0;
        return;
    }

    // DEBUG_TEXTURE_NORMAL - Show normal map textures
    if (debug_mode == DEBUG_TEXTURE_NORMAL) {
        mctx.color.xyz = fma(normalize(mat.normal_roughness.xzy), vec3(0.5), vec3(0.5));
        mctx.roughness = 0.7;
        mctx.metallic = 0.0;
        return;
    }

    // DEBUG_TEXTURE_ROUGHNESS - Show roughness from textures
    if (debug_mode == DEBUG_TEXTURE_ROUGHNESS) {
        mctx.color.xyz = vec3(mat.normal_roughness.a);
        mctx.roughness = 0.7;
        mctx.metallic = 0.0;
        return;
    }

    // DEBUG_VERTEX_NORMALS - Show terrain vertex normals
    if (debug_mode == DEBUG_VERTEX_NORMALS) {
        mctx.color.xyz = mctx.vertex_normal * 0.5 + 0.5;
        mctx.roughness = 0.7;
        mctx.metallic = 0.0;
        return;
    }

    // DEBUG_BILERP - Show bilinear interpolation areas
    if (debug_mode == DEBUG_BILERP) {
        mctx.color.xyz = vec3(bilerp ? 1.0 : 0.0);
        mctx.roughness = 0.7;
        mctx.metallic = 0.0;
        return;
    }

    // DEBUG_TEXTURE_IDS - Show texture IDs
    if (debug_mode == DEBUG_TEXTURE_IDS) {
        mctx.color.xyz = vec3(float(texture_ids.x) / 32.0, float(texture_ids.y) / 32.0, 0.0);
        mctx.roughness = 0.7;
        mctx.metallic = 0.0;
        return;
    }

    // DEBUG_TOTAL_WEIGHT - Show total weight (should be ~1.0)
    if (debug_mode == DEBUG_TOTAL_WEIGHT) {
        mctx.color.xyz = vec3(mat.total_weight / 2.0);
        mctx.roughness = 0.7;
        mctx.metallic = 0.0;
        return;
    }

    if (debug_mode == 17) {
        mctx.color.xyz = vec3(mat.normal_depth);
        mctx.roughness = 0.7;
        mctx.metallic = 0.0;
        return;
    }
}

