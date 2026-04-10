vec3 toneMapExponentialCurve(vec3 color) {
    return vec3(1.0) - exp(-color);
}

// Narkowicz ACES fitted approximation
vec3 toneMapACES(vec3 color) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}

// Hable / Uncharted 2 filmic curve
vec3 toneMapFilmic(vec3 x) {
    const float A = 0.15;
    const float B = 0.50;
    const float C = 0.10;
    const float D = 0.20;
    const float E = 0.02;
    const float F = 0.30;
    const float W = 11.2;

    vec3 curr = ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
    float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
    return clamp(curr / white, 0.0, 1.0);
}

// Curve only (linear HDR in). Apply linear exposure *before* calling (Filament-style order).
vec3 toneMappingCurve(vec3 color) {
#if defined(ENGINE_SETTINGS_TONEMAPPER_ACES)
    return toneMapACES(color);
#elif defined(ENGINE_SETTINGS_TONEMAPPER_FILMIC)
    return toneMapFilmic(color);
#else
    return toneMapExponentialCurve(color);
#endif
}

// Legacy helper: exposure baked into one call (e.g. if caller does not split exposure).
vec3 toneMapping(vec3 color, float exposure) {
    return toneMappingCurve(color * exposure);
}
