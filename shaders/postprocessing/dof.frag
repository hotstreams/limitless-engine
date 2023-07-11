Limitless::GLSL_VERSION
Limitless::Extensions

#include "../pipeline/scene.glsl"
#include "../functions/reconstruct_position.glsl"

in vec2 fs_uv;

out vec3 color;

uniform sampler2D depth_texture;
uniform sampler2D focus_texture;
uniform sampler2D unfocus_texture;

uniform vec2 uv_focus;
uniform vec2 distance;

void main() {
    vec2 uv = fs_uv;

    float far = getCameraFarPlane();

    vec3 position = reconstructPosition(uv, texture(depth_texture, uv).r);

    vec3 focus_position = reconstructPosition(uv_focus, texture(depth_texture, uv_focus).r);

    vec3 uf = texture(unfocus_texture, uv).rgb;
    vec3 f = texture(focus_texture, uv).rgb;

    float blur = smoothstep(distance.x, distance.y, length(position - focus_position));

    color  = mix(f, uf, blur);
}