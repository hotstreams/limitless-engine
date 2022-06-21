uniform sampler2D refraction_texture;

float f0ToIor(float f0) {
    float r = sqrt(f0);
    return (1.0 + r) / (1.0 - r);
}

vec3 refractionSolidSphere(const vec3 N, vec3 r, float etaIR, float etaRI) {
    float thickness = 0.5;
    r = refract(r, N, etaIR);
    float NoR = dot(N, r);
    float d = thickness * -NoR;
    vec3 position = vec3(getVertexPosition() + r * d);
    return position;
}

vec3 computeRefraction(const vec3 N, float metallic, const vec3 baseColor, float IoR, float absorption) {
    #if !defined (MATERIAL_IOR)
       vec3 F0 = computeF0(baseColor, metallic, 1.0);
       IoR = f0ToIor(F0.g);
    #endif

    const float airIor = 1.0;

    float etaIR = airIor / IoR;
    float etaRI = IoR / airIor;

    vec3 p = refractionSolidSphere(N, -normalize(getCameraPosition() - getVertexPosition()), etaIR, etaRI);
    vec4 p_cs = getViewProjection() * vec4(p, 1.0);
    p_cs.xy = p_cs.xy * (0.5 / p_cs.w) + 0.5;

    vec3 Ft = texture(refraction_texture, p_cs.xy).rgb;

    #if !defined (MATERIAL_IOR)
        Ft *= baseColor;
    #endif

    #if defined (MATERIAL_ABSORPTION)
        Ft *= 1.0 - absorption;
    #endif

    return Ft;
}
