ENGINE::COMMON

#include "./scene.glsl"

in vec2 uv;

out vec3 color;

uniform int width;

uniform sampler2D outline_texture;

void main() {
    bool inside = false;
    int count = 0;
    float coverage;
    float dist = 1e6;

    uint origin_mask = uint(texture(outline_texture, uv).a * 65535.0);

    for (int y = -width; y <= width; ++y) {
        for (int x = -width; x <= width; ++x) {
            uint umask = uint(texture(outline_texture, uv + vec2(x, y) * 1.0 / getResolution()).a * 65535.0);
            float mask = umask != origin_mask ? 0.0 : 1.0;

            coverage += mask;
            if (mask >= 0.5) {
                dist = min(dist, sqrt(x * x + y * y));
            }
            if (x == 0 && y == 0) {
                inside = (mask > 0.5);
            }
            count += 1;
        }
    }

    coverage /= count;

    float a;
    if (inside) {
        a = min(1.0, (1.0 - coverage) / 0.75);
    } else {
        const float solid = 0.3 * float(width);
        const float fuzzy = float(width) - solid;
        a = 1.0 - min(1.0, max(0.0, dist - solid) / fuzzy);
    }

    color = texture(outline_texture, uv).rgb * a;
}