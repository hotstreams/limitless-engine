float hashf(float f) {
    return fract(sin(f) * 1e4);
}

float hashv2(vec2 v) {
    return fract(1e4 * sin(17.0 * v.x + v.y * 0.1) * (0.1 + abs(sin(v.y * 13.0 + v.x))));
}

vec3 noise(vec2 x) {
    vec2 f = fract(x);

    vec2 u = f*f*f*(f*(f*6.0-15.0)+10.0);
    vec2 du = 30.0*f*f*(f*(f-2.0)+1.0);

    vec2 p = floor(x);

    float a = hashv2(p+vec2(0,0));
    float b = hashv2(p+vec2(1,0));
    float c = hashv2(p+vec2(0,1));
    float d = hashv2(p+vec2(1,1));

    float k0 = a;
    float k1 = b - a;
    float k2 = c - a;
    float k3 = a - b - c + d;

    return vec3( k0 + k1 * u.x + k2 * u.y + k3 * u.x * u.y, du * ( vec2(k1, k2) + k3 * u.yx) );
}

float value_noise(vec2 p) {
    float a = 0.0;
    float b = 1.0;
    vec2  d = vec2(0.0);

    for( int i=0; i < 16; i++ ) {
        vec3 n = noise(p);
        d += n.yz;
        a += b * n.x / (1.0 + dot(d,d));
        b *= 0.5;
        p = mat2( vec2(0.8, -0.6), vec2(0.6, 0.8) ) * p * 2.0;
    }
    a *= 0.9;
    return a;
}