ENGINE::COMMON

out vec3 color;

in vec2 uv;

uniform sampler2D image;
uniform float threshold;

void main() {
    vec3 source = texture(image, uv).rgb;
    // Soft-knee bloom thresholding (UE-style).
    // This avoids hard popping when values hover around the threshold.
    //
    // We use a fixed knee ratio to avoid adding new settings:
    // knee = threshold * 0.5
    float knee = threshold * 0.5;

    // Use luminance for thresholding, but preserve source color.
    float luma = dot(source, vec3(0.2126, 0.7152, 0.0722));

    // If knee is ~0, fall back to hard threshold.
    if (knee <= 1e-6) {
        float hard = max(luma - threshold, 0.0);
        color = source * (hard / max(luma, 1e-6));
        return;
    }

    // Smooth transition between (threshold - knee) and (threshold + knee)
    float x = max(luma - (threshold - knee), 0.0);
    float soft = clamp(x, 0.0, 2.0 * knee);
    soft = (soft * soft) / (4.0 * knee + 1e-6);

    // Combine hard and soft components, then scale original color.
    float contribution = max(luma - threshold, soft);
    color = source * (contribution / max(luma, 1e-6));
}