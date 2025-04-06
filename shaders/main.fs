struct ShadowData {
    mat4 lightFromWorldMatrix;
    vec4 lightFromWorldZ;
    vec4 scissorNormalized;
    float texelSizeAtOneMeter;
    float bulbRadiusLs;
    float nearOverFarMinusNear;
    float normalBias;
    bool elvsm;
    uint layer;
    uint reserved1;
    uint reserved2;
};

layout(std140, binding = 0) uniform ShadowUniforms {
    ShadowData shadows[4];  // Support up to 4 cascades
};

// Get shadow position in light space
vec4 getShadowPosition(int cascade) {
    return shadows[cascade].lightFromWorldMatrix * vec4(getWorldPosition(), 1.0);
}

// Sample shadow map with PCF
float sampleShadowMap(sampler2DArray shadowMap, int cascade, vec4 shadowPosition) {
    // Get shadow map dimensions
    ivec2 shadowMapSize = textureSize(shadowMap, 0).xy;
    
    // Convert to shadow map coordinates
    vec3 shadowCoord = shadowPosition.xyz / shadowPosition.w;
    shadowCoord.xy = shadowCoord.xy * 0.5 + 0.5;
    shadowCoord.z = shadowCoord.z * 0.5 + 0.5;
    
    // Apply scissor region
    shadowCoord.xy = shadows[cascade].scissorNormalized.xy + 
                     shadowCoord.xy * shadows[cascade].scissorNormalized.zw;
    
    // Early exit if outside shadow map
    if (shadowCoord.x < 0.0 || shadowCoord.x > 1.0 || 
        shadowCoord.y < 0.0 || shadowCoord.y > 1.0) {
        return 1.0;
    }
    
    // PCF sampling
    float shadow = 0.0;
    float texelSize = 1.0 / float(shadowMapSize.x);
    
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            vec2 offset = vec2(x, y) * texelSize;
            float depth = texture(shadowMap, vec3(shadowCoord.xy + offset, float(cascade))).r;
            shadow += step(shadowCoord.z, depth);
        }
    }
    
    return shadow / 9.0;
}

// Get shadow cascade index based on view-space depth
int getShadowCascade() {
    float depth = getViewSpaceDepth();
    vec4 splits = frameUniforms.cascadeSplits;
    
    if (depth <= splits.x) return 0;
    if (depth <= splits.y) return 1;
    if (depth <= splits.z) return 2;
    if (depth <= splits.w) return 3;
    return 3; // Use last cascade for everything beyond
}

void main() {
    // Calculate shadows
    float shadow = 1.0;
    if (hasDirectionalLight) {
        int cascade = getShadowCascade();
        vec4 shadowPosition = getShadowPosition(cascade);
        shadow = sampleShadowMap(shadowMap, cascade, shadowPosition);
    }
    
    // Apply shadows to lighting
    color *= shadow;
} 