float f0ToIor(float f0) {
    float r = sqrt(f0);
    return (1.0 + r) / (1.0 - r);
}

vec3 refractionSolidSphere(const vec3 N, vec3 r, float etaIR, float etaRI, vec3 position) {
    float thickness = 0.5;
    r = refract(r, N, etaIR);
    float NoR = dot(N, r);
    float d = thickness * -NoR;
    position = vec3(position + r * d);
    return position;
}
