ENGINE::COMMON

in vec2 vs_uv;
in vec4 vs_color;

out vec4 FragColor;

// Signed distance field, see FT_RENDER_MODE_SDF.
uniform sampler2D bitmap;
// Atlas pixels that map to the full distance field range.
uniform float spread;

// 0 draws shadow and outline, 1 draws glyphs over them.
uniform int effect_pass;

// Effects are disabled by zero alpha. Sizes are in screen pixels.
uniform vec4 shadow_color;
// y points down.
uniform vec2 shadow_offset;
uniform float shadow_softness;
uniform vec4 outline_color;
uniform float outline_width;

// Distance to the glyph outline at uv in screen pixels, positive inside.
float distanceAt(vec2 uv, float atlas_pixels_per_screen_pixel) {
    float field = texture(bitmap, uv).r * 255.0;
    return (field - 128.0) / 128.0 * spread / atlas_pixels_per_screen_pixel;
}

void main() {
    // Glyph quads are axis aligned, so each axis changes only one UV coordinate.
    vec2 uv_per_pixel = fwidth(vs_uv);
    vec2 atlas_pixels = uv_per_pixel * vec2(textureSize(bitmap, 0));
    float atlas_pixels_per_screen_pixel = max(0.5 * (atlas_pixels.x + atlas_pixels.y), 1e-4);

    float dist = distanceAt(vs_uv, atlas_pixels_per_screen_pixel);

    if (effect_pass == 1) {
        FragColor = vec4(vs_color.rgb, clamp(dist + 0.5, 0.0, 1.0) * vs_color.a);
        return;
    }

    float shadow_alpha = 0.0;
    if (shadow_color.a > 0.0) {
        // Atlas v grows downwards on screen, as the offset does.
        float shadow_dist = distanceAt(vs_uv - shadow_offset * uv_per_pixel, atlas_pixels_per_screen_pixel);
        float softness = max(shadow_softness, 0.5);
        shadow_alpha = smoothstep(-softness, softness, shadow_dist) * shadow_color.a;
    }

    float outline_alpha = 0.0;
    if (outline_color.a > 0.0) {
        outline_alpha = clamp(dist + outline_width + 0.5, 0.0, 1.0) * outline_color.a;
    }

    // Outline over shadow.
    float alpha = outline_alpha + shadow_alpha * (1.0 - outline_alpha);
    vec3 color = (outline_color.rgb * outline_alpha + shadow_color.rgb * shadow_alpha * (1.0 - outline_alpha))
        / max(alpha, 1e-5);

    FragColor = vec4(color, alpha * vs_color.a);
}
