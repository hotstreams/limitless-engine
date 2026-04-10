ENGINE::COMMON

in vec2 uv;

out vec3 color;

uniform sampler2D screen_texture;
uniform vec2 viewport_pixels;
/// 0 = output full RGB; 1 = replicate red only (SSAO buffer stores packed depth in GB — full RGB looks magenta/purple in debug).
uniform float quad_red_channel_only;

void main() {
    // UV from fragment position avoids the two-triangle quad seam (interpolated varyings
    // are discontinuous along the diagonal; half-res → full blits show it as a hard edge).
    vec2 tuv = gl_FragCoord.xy / viewport_pixels;
    vec3 tex = textureLod(screen_texture, tuv, 0.0).rgb;
    color = quad_red_channel_only > 0.5 ? vec3(tex.r) : tex;
}