float circle(in vec2 uv, in float r){
    vec2 dist = uv - vec2(0.5);
    return 1.0 - smoothstep(r - (r * 0.3), r + (r * 0.3), dot(dist, dist) * 4.0);
}